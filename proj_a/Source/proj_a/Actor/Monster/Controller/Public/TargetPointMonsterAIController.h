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

	TWeakObjectPtr<ACharacter> TargetCharacter;
protected:
	virtual void SimulateMovement(float delta_time) override;
};
