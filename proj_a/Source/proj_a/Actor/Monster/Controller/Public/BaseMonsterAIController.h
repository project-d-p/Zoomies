#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseMonsterCharacter.h"
#include "BB_Monster.h"
#include "BT_Monster.h"
#include "Navigation/CrowdAgentInterface.h"
#include "BaseMonsterAIController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMonsterAI, Log, All);

struct FEnvQueryResult;

UCLASS()
class PROJ_A_API ABaseMonsterAIController : public AAIController, public ICrowdAgentInterface
{
	GENERATED_BODY()

public:
	ABaseMonsterAIController();
	int32 GetMonsterId() const { return Cast<ABaseMonsterCharacter>(GetCharacter())->MonsterId; }
	void RemovePawnAndController();
	virtual void SimulateMovement(float delta_time) {}

protected:
	UPROPERTY()
	UBT_Monster* BehaviorTreeAsset;
	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorComp;
	
	UPROPERTY()
	UBB_Monster* BlackboardAsset;
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComp;
};
