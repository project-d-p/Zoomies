#include "ResultLevelGameState.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "EngineUtils.h"
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

void AResultLevelGameState::NotifyPlayersAllTraveled()
{
	this->SetPlayerScores();
	this->SetMyRank();
	NotifyAllScoresCalculated(FinalScoreDataArray);
}

void AResultLevelGameState::NotifyAllScoresCalculated_Implementation(
	const TArray<FFinalScoreData>& InFinalScoreDataArray)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AResultWidgetActor::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		AResultWidgetActor* ResultActor = Cast<AResultWidgetActor>(FoundActors[0]);
		if (ResultActor)
		{
			ResultActor->StartWidget(InFinalScoreDataArray);
		}
	}
}

void AResultLevelGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void AResultLevelGameState::SetMyRank()
{
	FinalScoreDataArray.Sort([](const FFinalScoreData& A, const FFinalScoreData& B) {
		float ScoreA = A.bIsDetected ? A.PublicTotalScore : A.PrivateTotalScore;
		float ScoreB = B.bIsDetected ? B.PublicTotalScore : B.PrivateTotalScore;
		return ScoreA > ScoreB;
	});
}

void AResultLevelGameState::SetPlayerScores()
{
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
		FFinalScoreData FinalScoreData;
		FinalScoreData = PlayerState->GetPlayerScoreData()->GetScore();
		FinalScoreDataArray.Add(FinalScoreData);
	}

	if (FinalScoreDataArray.Num() < 4)
	{
		for (int i = FinalScoreDataArray.Num(); i < 4; i++)
		{
			FFinalScoreData FinalScoreData;
			FinalScoreData.PlayerName = TEXT("NONE");
			FinalScoreDataArray.Add(FinalScoreData);
		}
	}
}

void AResultLevelGameState::BeginPlay()
{
	Super::BeginPlay();

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
}

void AResultLevelGameState::SetWinner()
{
	FString Winner = FinalScoreDataArray[0].PlayerName;
	UE_LOG(LogTemp, Warning, TEXT("Winner : %s"), *Winner);

	FString MyName = GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName();

	for (TActorIterator<ADPCharacter> It(GetWorld()); It; ++It)
	{
		ADPCharacter* Character = *It;
		if (Character)
		{
			if (Character->GetPlayerState()->GetPlayerName() == Winner)
			{
				Character->SetCrownMesh();
			}
		}
	}
}