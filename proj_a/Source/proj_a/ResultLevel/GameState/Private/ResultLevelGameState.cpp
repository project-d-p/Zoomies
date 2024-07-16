#include "ResultLevelGameState.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"

AResultLevelGameState::AResultLevelGameState()
{
}

void AResultLevelGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
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
