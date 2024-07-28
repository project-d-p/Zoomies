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

	static ConstructorHelpers::FObjectFinder<USoundBase> SHOTSOUND
	(TEXT("/Game/sounds/effect/character/shot/laser_gun_fire_001_Cue.laser_gun_fire_001_Cue"));
	if (SHOTSOUND.Succeeded()) {
		ShotSound = SHOTSOUND.Object;
	}
	else
		ShotSound = nullptr;

	static ConstructorHelpers::FObjectFinder<USoundBase> ANIMALGETDAMAGEDSOUND
	(TEXT("/Game/sounds/effect/animal/damaged/hit_machine_Cue.hit_machine_Cue"));
	if (ANIMALGETDAMAGEDSOUND.Succeeded()) {
		AnimalGetDamagedSound = ANIMALGETDAMAGEDSOUND.Object;
	}
	else
		AnimalGetDamagedSound = nullptr;

	static ConstructorHelpers::FObjectFinder<USoundBase> ANIMALFAINTSOUND
	(TEXT("/Game/sounds/effect/animal/faint/robot_shut_down_Cue.robot_shut_down_Cue"));
	if (ANIMALFAINTSOUND.Succeeded()) {
		AnimalFaintSound = ANIMALFAINTSOUND.Object;
	}
	else
		AnimalFaintSound = nullptr;
}

void USoundComponent::PlayJumpSound() const
{
	UGameplayStatics::PlaySound2D(GetWorld(), JumpSound);
}

void USoundComponent::PlayShotSound() const
{
	UGameplayStatics::PlaySound2D(GetWorld(), ShotSound);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("soundComponent"));
}

void USoundComponent::PlayAnimalGetDamagedSound() const
{
	UGameplayStatics::PlaySound2D(GetWorld(), AnimalGetDamagedSound);
}

void USoundComponent::PlayAnimalFaintSound() const
{
	UGameplayStatics::PlaySound2D(GetWorld(), AnimalFaintSound);
}
