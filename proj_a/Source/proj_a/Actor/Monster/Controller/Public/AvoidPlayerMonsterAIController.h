#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterAIController.h"
#include "AvoidPlayerMonsterAIController.generated.h"

UCLASS()
class PROJ_A_API AAvoidPlayerMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()

private:
	bool IsMovingToTarget() const;
	void MoveToRandomTargetLocation();

	UPROPERTY()
	FTimerHandle EQSQueryTimerHandle;
	UPROPERTY()
	FTimerHandle DeSpawnTimerHandle;

	FVector CachedSafeLocation;

	void RunEQSQuery();
	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);
protected:
	virtual void BeginPlay() override;
};