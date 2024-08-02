#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoundComponent.generated.h"

UCLASS()
class USoundComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USoundComponent();

	void PlayJumpSound() const;
	void PlayShotSound() const;
	void PlayAnimalGetDamagedSound() const;
	void PlayAnimalFaintSound() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* JumpSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ShotSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* AnimalGetDamagedSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* AnimalFaintSound;
};
