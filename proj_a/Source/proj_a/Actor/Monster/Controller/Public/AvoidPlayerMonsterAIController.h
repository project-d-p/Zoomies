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

	const float EQSQueryTime = 0.5;
	const float DeSpawnTime = 10.0f;

	float elapsed_EQSQueryTime = 0;
	float elapsed_DeSpawnTime = 0;
	
	FVector CachedSafeLocation;

	void RunEQSQuery();
	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);
protected:
	virtual void BeginPlay() override;
	virtual void SimulateMovement(float delta_time) override;
};