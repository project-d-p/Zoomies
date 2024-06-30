#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterAIController.h"
#include "TargetPointMonsterAIController.generated.h"

UCLASS()
class PROJ_A_API ATargetPointMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()

private:
	void MoveToTargetLocation();

protected:
	virtual void Tick(float DeltaSeconds) override;
};
