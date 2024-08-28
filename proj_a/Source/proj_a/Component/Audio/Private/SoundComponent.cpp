#include "SoundComponent.h"

#include "Kismet/GameplayStatics.h"

USoundComponent::USoundComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(this);
	AudioComponent->bAutoActivate = false;

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

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> AttenuationSettings
	(TEXT("/Game/sounds/dp_animalSoundAttenuation.dp_animalSoundAttenuation"));
	if (AttenuationSettings.Succeeded()) {
		AudioComponent->AttenuationSettings = AttenuationSettings.Object;
	}
}

void USoundComponent::PlayJumpSound() const
{
	AudioComponent->SetSound(JumpSound);
	AudioComponent->Play();
	//UGameplayStatics::PlaySound2D(GetWorld(), JumpSound);
}

void USoundComponent::PlayShotSound() const
{
	AudioComponent->SetSound(ShotSound);
	AudioComponent->Play();
}

void USoundComponent::PlayAnimalGetDamagedSound() const
{
	AudioComponent->SetSound(AnimalGetDamagedSound);
	AudioComponent->Play();
}

void USoundComponent::PlayAnimalFaintSound() const
{
	AudioComponent->SetSound(AnimalFaintSound);
	AudioComponent->Play();
}