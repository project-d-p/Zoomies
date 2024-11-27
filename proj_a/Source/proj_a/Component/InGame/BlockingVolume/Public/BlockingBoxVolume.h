#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockingBoxVolume.generated.h"

UCLASS()
class ABlockingBoxVolume : public AActor
{
	GENERATED_BODY()
public:
	ABlockingBoxVolume();

	void DeactiveBlockingVolume(bool& bWallDisappear);
	void Clear();

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyComponent();
	
private:
	UPROPERTY()
	UStaticMeshComponent* BlockingBox;
	FTimerHandle DeactiveTimer;
};
