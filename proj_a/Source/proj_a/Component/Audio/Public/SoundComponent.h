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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* JumpSound;
};
