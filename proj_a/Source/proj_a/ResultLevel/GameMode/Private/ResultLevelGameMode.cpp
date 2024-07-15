#include "ResultLevelGameMode.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "ResultLevelGameState.h"

AResultLevelGameMode::AResultLevelGameMode()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
	PlayerStateClass = ADPPlayerState::StaticClass();
	GameStateClass = AResultLevelGameState::StaticClass();
}

AResultLevelGameMode::~AResultLevelGameMode()
{
}

void AResultLevelGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(NewPlayer);
	if (PlayerController)
	{
		UPrivateScoreManager* ScoreManagerComponent = PlayerController->GetPrivateScoreManagerComponent();
		if (ScoreManagerComponent)
		{
			int32 PlayerScore = ScoreManagerComponent->GetPrivatePlayerScore();
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Score[%s] : %d"), *PlayerController->PlayerState->GetPlayerName(), PlayerScore);
		}
	}
}

void AResultLevelGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
