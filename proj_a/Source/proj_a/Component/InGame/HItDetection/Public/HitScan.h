#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DPPlayerController.h"
#include "HitScan.generated.h"

UCLASS()
class UHitScan : public UActorComponent
{
	GENERATED_BODY()
public:
	bool HitDetect(const ADPCharacter* controller, const FVector& start, const FRotator& direction, const float& distance, FHitResult&
	               result);
	virtual ~UHitScan() override;
};
