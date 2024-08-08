#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterAIController.h"
#include "ChasePlayerMonsterAIController.generated.h"

UCLASS()
class PROJ_A_API AChasePlayerMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()

public:
	void SetRandomPlayerIndex();
	int32 GetPlayerIndex() const { return PlayerIndex; }
	
private:
	int32 PlayerIndex = -1;
	void MoveToPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void SimulateMovement(float delta_time) override;
};
