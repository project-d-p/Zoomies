#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterAIController.h"
#include "ChasePlayerMonsterAIController.generated.h"

UCLASS()
class PROJ_A_API AChasePlayerMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()

private:
	int32 PlayerIndex = 0;
	void MoveToPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
