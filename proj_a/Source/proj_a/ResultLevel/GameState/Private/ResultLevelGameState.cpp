#include "ResultLevelGameState.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "EnumTypes.h"
#include "PathManager.h"
#include "ResultWidgetActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AResultLevelGameState::AResultLevelGameState()
{
	bReplicates = true;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> RESULT_WIDGET
	(PathManager::GetWidgetPath(EWidget::RESULT_WIDGET));
	if (RESULT_WIDGET.Succeeded())
	{
		CalculateWidget = RESULT_WIDGET.Class;
	}
	
	static ConstructorHelpers::FClassFinder<UUserWidget> CALCULATE_WIDGET
	(PathManager::GetWidgetPath(EWidget::CALCULATE_WIDGET));
	if (CALCULATE_WIDGET.Succeeded())
	{
		ResultWidget = CALCULATE_WIDGET.Class;
	}
	
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
}

void AResultLevelGameState::MulticastPlayersAllTraveled_Implementation()
{
	NotifyPlayersAllTraveled();
}

void AResultLevelGameState::NotifyPlayersAllTraveled_Implementation()
{
	if (HasAuthority())
	{
		this->SetPlayerScores();
		this->SetMyRank();
		this->isAllSet = true;
		this->OnRep_IsAllSet();
	}
	
	ADPPlayerController* Controller = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (Controller)
	{
		Controller->SwitchLevelComponent(ELevelComponentType::RESULT);
	}
	ADPCharacter* Character = Cast<ADPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Character)
	{
		Character->SetReplicatingMovement(true);
	}
}

void AResultLevelGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

TArray<FAnimalList> AResultLevelGameState::GetCapturedAnimals(/*ADPPlayerController* Controller*/ TArray<TArray<EAnimal>> InCapturedAnimals)
{
	TArray<FAnimalList> CapturedAnimals;
	
	for (const TArray<EAnimal>& Animals : InCapturedAnimals)
	{
		FAnimalList AnimalList;
		AnimalList.Animals = Animals;
		CapturedAnimals.Add(AnimalList);
	}
	
	return CapturedAnimals;
}

void AResultLevelGameState::SetMyRank()
{
	PlayerScores.Sort([](const FPlayerScore& A, const FPlayerScore& B) {
		return A.Scores[4] > B.Scores[4];
	});

	int rank = 1;
	PlayerScores[0].Rank = rank;

	for (int i = 1; i < PlayerScores.Num(); i++)
	{
		if (PlayerScores[i - 1].Scores[4] != PlayerScores[i].Scores[4])
		{
			rank += 1;
		}
		PlayerScores[i].Rank = rank;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADPPlayerController* PlayerController = Cast<ADPPlayerController>(*Iterator);
		if (!PlayerController)
		{
			continue;
		}	
		ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
		if (!PlayerState)
		{
			continue;
		}
		FPlayerScore* PlayerScore = PlayerScores.FindByPredicate([PlayerState](const FPlayerScore& Score) {
			return Score.PlayerName == PlayerState->GetPlayerName();
		});
		if (!PlayerScore)
		{
			continue;
		}
		PlayerState->Rank = PlayerScore->Rank;
		if (PlayerController->IsLocalController())
		{
			MyRank = PlayerScore->Rank;
		}
	}
}

void AResultLevelGameState::SetPlayerScores()
{
	FPlayerScore PlayerScore;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADPPlayerController* PlayerController = Cast<ADPPlayerController>(*Iterator);
		if (!PlayerController)
		{
			continue ;
		}
		ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
		if (!PlayerState)
		{
			continue ;
		}
		PlayerScore.PlayerName = PlayerState->GetPlayerName();
		PlayerScore.PlayerJob = EPlayerJob::JOB_ARCHAEOLOGIST /*PlayerState->GetPlayerJob()*/;
		FFinalScoreData FD = PlayerState->GetFinalScoreData();
		PlayerScore.Scores = this->CalculateScores(FD.CapturedAnimals, FD.ScoreDatas /*PlayerController*/);
		PlayerScore.CapturedAnimals = this->GetCapturedAnimals(FD.CapturedAnimals/*PlayerController*/);
		PlayerScore.bIsDetected = FD.bIsDetected;
		PlayerScores.Add(PlayerScore);
	}

	if (PlayerScores.Num() < 4)
	{
		FPlayerScore DummyPlayerScore;

		for (int i = PlayerScores.Num(); i < 4; i++)
		{
			DummyPlayerScore.PlayerName = FText::FromString(TEXT("Player")).ToString() + FString::FromInt(i + 1);
			DummyPlayerScore.PlayerJob = EPlayerJob::JOB_ARCHAEOLOGIST;
			DummyPlayerScore.Scores = {0, 0, 0, 0, 0};
			DummyPlayerScore.CapturedAnimals = {};
			DummyPlayerScore.bIsDetected = false;
			PlayerScores.Add(DummyPlayerScore);
		}
	}
}

TArray<int32> AResultLevelGameState::CalculateScores(/* ADPPlayerController* Controller */ TArray<TArray<EAnimal>> InCapturedAnimals, TArray<FScoreData> InScores)
{
	TArray<int32> Scores;

	Scores.SetNum(5);

	// UPrivateScoreManager* ScoreManager = Controller->GetPrivateScoreManagerComponent();
	// if (!ScoreManager)
	// {
	// 	return Scores;
	// }

	TArray<TArray<EAnimal>> CapturedAnimals = InCapturedAnimals /*ScoreManager->GetCapturedAnimals()*/;
	TArray<FScoreData> ScoreDatas = InScores /*ScoreManager->GetScoreDatas()*/;

	int BaseScore = 0;
	int BaseScoreAlpha = 0;
	int AddMulScore = 0;
	int MulMulScore = 1;
	
	for (int32 i = 0; i < ScoreDatas.Num(); i++)
	{
		BaseScore += CapturedAnimals[i].Num() * 100;
		BaseScoreAlpha += ScoreDatas[i].baseScore;
		AddMulScore += ScoreDatas[i].addMulScore;
		MulMulScore *= ScoreDatas[i].mulMulScore;
	}

	Scores[0] = BaseScore;
	Scores[1] = BaseScoreAlpha;
	Scores[2] = BaseScoreAlpha * (AddMulScore == 0 ? 1 : AddMulScore);
	Scores[3] = Scores[2] * MulMulScore;
	Scores[4] = Scores[3];
	
	return Scores;
}

void AResultLevelGameState::BeginPlay()
{
	Super::BeginPlay();
	
	// this->SetMyRank();
}

void AResultLevelGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// ���� �ν��Ͻ��� ���� ���� ����
    UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
    if (GameInstance)
    {
    	IOnlineSessionPtr SessionInt = GameInstance->GetOnlineSessionInterface();
    	if (SessionInt.IsValid())
    	{
    		SessionInt->DestroySession(NAME_GameSession);
    	}
    }
}

void AResultLevelGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AResultLevelGameState, PlayerScores);
	DOREPLIFETIME(AResultLevelGameState, isAllSet);
}

void AResultLevelGameState::OnRep_IsAllSet()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AResultWidgetActor::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		AResultWidgetActor* ResultActor = Cast<AResultWidgetActor>(FoundActors[0]);
		if (ResultActor)
		{
			ResultActor->StartWidget();
		}
	}
}
