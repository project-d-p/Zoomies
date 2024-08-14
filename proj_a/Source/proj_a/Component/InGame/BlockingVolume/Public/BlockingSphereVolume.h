#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "BlockingSphereVolume.generated.h"

UCLASS()
class ABlockingSphereVolume : public AActor
{
	GENERATED_BODY()
public:
	ABlockingSphereVolume();

	void DeactiveBlockingVolume();

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* BlockingSphere;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* InnerSphere;
};
