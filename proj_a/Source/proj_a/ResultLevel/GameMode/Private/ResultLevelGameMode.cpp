#include "ResultLevelGameMode.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "ResultLevelGameState.h"

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

void AResultLevelGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void AResultLevelGameMode::Broadcast_Implementation(AActor* Sender, const FString& Msg, FName Type)
{
	// Super::Broadcast(Sender, Msg, Type);
	ADPPlayerController* SenderController = Cast<ADPPlayerController>(Sender);
	FString SenderName = SenderController ? SenderController->PlayerState->GetPlayerName() : TEXT("Server");
	
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		ADPPlayerController* PlayerController = Cast<ADPPlayerController>(*Iterator);
		if (PlayerController)
		{
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Broadcast_Implementation : %s"), *Msg);
			PlayerController->ReceiveChatMessage(SenderName, Msg);
		}
	}
}

void AResultLevelGameMode::Logout(AController* Exiting)
{
	// Clear the session when the player leaves the game
	Super::Logout(Exiting);
}

void AResultLevelGameMode::SpawnNewPlayerPawn(AController* PC)
{
	// 새 캐릭터 생성 없이 기존 캐릭터 재사용 로직 구현
	static int i = 0;
	if (i == 2)
	{
		i = 0;
	}
	FVector Location[2] = {
		{-527.514681,-128.409500,85.462503},
		{-527.514681,138.648437,85.462503}
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

/* Seamless Travel : Reuse PlayerControllers */
void AResultLevelGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	SpawnNewPlayerPawn(C);
}

void AResultLevelGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}