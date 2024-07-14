#include "SoundComponent.h"

#include "Kismet/GameplayStatics.h"

USoundComponent::USoundComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset
	(TEXT("/Game/sounds/effect/character/jump_Cue.jump_Cue"));
	if (SoundAsset.Succeeded()) {
		JumpSound = SoundAsset.Object;
	}
	else
		JumpSound = nullptr;
}

void USoundComponent::PlayJumpSound() const
{
	UGameplayStatics::PlaySound2D(GetWorld(), JumpSound);
}
