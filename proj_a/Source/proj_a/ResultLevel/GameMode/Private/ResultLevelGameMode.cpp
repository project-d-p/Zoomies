#include "ResultLevelGameMode.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "LocalizationDescriptor.h"
#include "ResultLevelGameState.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AResultLevelGameMode::AResultLevelGameMode()
{
	bUseSeamlessTravel = true;

	PrimaryActorTick.bCanEverTick = true;
	
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
	PlayerStateClass = ADPPlayerState::StaticClass();
	GameStateClass = AResultLevelGameState::StaticClass();
}

AResultLevelGameMode::~AResultLevelGameMode()
{
}

/* Seamless Travel : Reuse PlayerController for Server */
void AResultLevelGameMode::PostSeamlessTravel()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (!GameInstance) return;

	FVector Location[2] = {
		{377.379388,44.683849,87.312484},
		{377.379388,75.275560,87.312482}
	};

	ADPCharacter* NewCharacter = GetWorld()->SpawnActor<ADPCharacter>(DefaultPawnClass, Location[0], FRotator::ZeroRotator);
	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(GameInstance->LocalController);
	
	if (PlayerController && PlayerController->IsLocalPlayerController())
	{
		if (!NewCharacter)
		{
			return ;
		}
		PlayerController->Possess(NewCharacter);
		PlayerController->SwitchLevelComponent(ELevelComponentType::RESULT);
	}
}

void AResultLevelGameMode::StartPlay()
{
	Super::StartPlay();

	this->SetPlayerScores();
	
	// for (auto& PlayerScore : PlayerScores)
	// {
	// 	FNetLogger::LogError(TEXT("PlayerName: %s"), *PlayerScore.PlayerName);
	// 	FNetLogger::LogError(TEXT("PlayerJob: %d"), (int32)PlayerScore.PlayerJob);
	// 	FNetLogger::LogError(TEXT("Scores: %d, %d, %d, %d, %d"), PlayerScore.Scores[0], PlayerScore.Scores[1], PlayerScore.Scores[2], PlayerScore.Scores[3], PlayerScore.Scores[4]);
	// 	FNetLogger::LogError(TEXT("bIsDetected: %d"), PlayerScore.bIsDetected);
	// }
}

void AResultLevelGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

TArray<int32> AResultLevelGameMode::CalculateScores(ADPPlayerController* Controller)
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

TArray<FAnimalList> AResultLevelGameMode::GetCapturedAnimals(ADPPlayerController* Controller)
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

void AResultLevelGameMode::SetPlayerScores()
{
	FString PlayerName("TEST");
	EPlayerJob PlayerJob = EPlayerJob::JOB_ARCHAEOLOGIST;
	TArray<int32> Scores = {0, 0, 0, 0, 0};
	TArray<FAnimalList> CapturedAnimals;
	bool bIsDetected = false;
	
	// for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	// {
	// 	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(*Iterator);
	// 	if (!PlayerController)
	// 	{
	// 		return ;
	// 	}
	// 	ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
	// 	if (!PlayerState)
	// 	{
	// 		return ;
	// 	}

	for (int i = 0; i < 4; i++) {
		FPlayerScore PlayerScore;
		
		// PlayerScore.PlayerName = PlayerState->GetPlayerName();
		// PlayerScore.PlayerJob = PlayerState->GetPlayerJob();
		// PlayerScore.Scores = this->CalculateScores(PlayerController);
		// PlayerScore.CapturedAnimals = this->GetCapturedAnimals(PlayerController);
		// PlayerScore.bIsDetected = false;

		/* Test */
		PlayerScore.PlayerName = PlayerName;
		PlayerName += "_";
		PlayerScore.PlayerJob = PlayerJob;
		PlayerScore.Scores = Scores;

		Scores[0] += 100;
		Scores[1] += 100;
		Scores[2] += 100;
		Scores[3] += 100;
		Scores[4] += 100;
		
		// PlayerScore.CapturedAnimals = this->GetCapturedAnimals(PlayerController);
		PlayerScore.bIsDetected = false;
		
		PlayerScores.Add(PlayerScore);
	}
}

/* Seamless Travel : Reuse PlayerController for Clients */
void AResultLevelGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	APlayerController* PC = Cast<APlayerController>(C);
	FNetLogger::LogError(TEXT("Handling seamless travel for: %d"), PC->GetUniqueID());
	if (PC && PC->Player)
	{
		// 기존 컨트롤러를 유지
		FNetLogger::LogError(TEXT("Keeping existing PlayerController for seamless travel: %s"), *PC->GetName());
        
		// 필요한 초기화만 수행
		InitSeamlessTravelPlayer(C);
		GenericPlayerInitialization(C);
        
		// 새 캐릭터 생성 없이 기존 캐릭터 재사용 로직 구현
		static int i = 1;
		if (i == 2)
		{
			i = 0;
		}
		FVector Location[2] = {
			{377.379388,44.683849,87.312484},
			{377.379388,75.275560,87.312482}
		};

		FVector SpawnLocation = Location[i++];  // 적절한 스폰 위치 설정
		ADPCharacter* NewCharacter = GetWorld()->SpawnActor<ADPCharacter>(DefaultPawnClass, SpawnLocation, FRotator::ZeroRotator);
		if (NewCharacter)
		{
			PC->Possess(NewCharacter);
		}
		ADPPlayerController* PlayerController = Cast<ADPPlayerController>(PC);
		if (PlayerController)
		{
			PlayerController->SwitchLevelComponent(ELevelComponentType::RESULT);
		}
	}
	else
	{
		// 플레이어 컨트롤러가 없는 경우에 대한 처리
		Super::HandleSeamlessTravelPlayer(C);
	}
}
