#include "ResultLevelGameState.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AResultLevelGameState::AResultLevelGameState()
{
}

void AResultLevelGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

TArray<FAnimalList> AResultLevelGameState::GetCapturedAnimals(ADPPlayerController* Controller)
{
	TArray<FAnimalList> CapturedAnimals;

	UPrivateScoreManager* ScoreManager = Controller->GetPrivateScoreManagerComponent();
	if (!ScoreManager)
	{
		return CapturedAnimals;
	}
	
	for (const TArray<EAnimal>& Animals : ScoreManager->GetCapturedAnimals())
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
	if (PlayerScores[0].PlayerName == GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName())
	{
		GetWorld()->GetFirstPlayerController()->PlayerState->SetScore(rank);
	}
	
	for (int i = 1; i < PlayerScores.Num(); i++)
	{
		if (PlayerScores[i - 1].Scores[4] != PlayerScores[i].Scores[4])
		{
			rank += 1;
		}
		if (PlayerScores[i].PlayerName == GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName())
		{
			GetWorld()->GetFirstPlayerController()->PlayerState->SetScore(rank);
		}
		PlayerScores[i].Rank = rank;
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
			return ;
		}
		ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
		if (!PlayerState)
		{
			return ;
		}
		PlayerScore.PlayerName = PlayerState->GetPlayerName();
		PlayerScore.PlayerJob = PlayerState->GetPlayerJob();
		PlayerScore.Scores = this->CalculateScores(PlayerController);
		PlayerScore.CapturedAnimals = this->GetCapturedAnimals(PlayerController);
		PlayerScore.bIsDetected = false;
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

TArray<int32> AResultLevelGameState::CalculateScores(ADPPlayerController* Controller)
{
	TArray<int32> Scores;

	Scores.SetNum(5);

	UPrivateScoreManager* ScoreManager = Controller->GetPrivateScoreManagerComponent();
	if (!ScoreManager)
	{
		return Scores;
	}
	FNetLogger::LogError(TEXT("ScoreManager is valid"));

	TArray<TArray<EAnimal>> CapturedAnimals = ScoreManager->GetCapturedAnimals();
	TArray<FScoreData> ScoreDatas = ScoreManager->GetScoreDatas();

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
	if (HasAuthority())
	{
		this->SetPlayerScores();
		this->SetMyRank();
	}
	
	for (auto& PlayerScore : PlayerScores)
	{
		FNetLogger::LogError(TEXT("PlayerName: %s"), *PlayerScore.PlayerName);
		FNetLogger::LogError(TEXT("PlayerJob: %d"), (int32)PlayerScore.PlayerJob);
		FNetLogger::LogError(TEXT("Scores: %d, %d, %d, %d, %d"), PlayerScore.Scores[0], PlayerScore.Scores[1], PlayerScore.Scores[2], PlayerScore.Scores[3], PlayerScore.Scores[4]);
		FNetLogger::LogError(TEXT("bIsDetected: %d"), PlayerScore.bIsDetected);
	}
	
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

	// 게임 인스턴스를 통해 세션 제거
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
}
