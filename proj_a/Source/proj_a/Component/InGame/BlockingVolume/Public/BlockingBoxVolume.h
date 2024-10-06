#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "BlockingBoxVolume.generated.h"

UCLASS()
class ABlockingBoxVolume : public AActor
{
	GENERATED_BODY()
public:
	ABlockingBoxVolume();

	void DeactiveBlockingVolume(bool& bWallDisappear);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyComponent();
	
private:
	UPROPERTY()
	UStaticMeshComponent* BlockingBox;
};
