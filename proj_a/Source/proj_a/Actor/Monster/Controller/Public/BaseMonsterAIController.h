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
	ABaseMonsterAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	int32 GetMonsterId() const
	{
		ABaseMonsterCharacter* MonsterCharacter = Cast<ABaseMonsterCharacter>(GetCharacter());
		if (MonsterCharacter == nullptr)
		{
			return -1;
		}
		return MonsterCharacter->MonsterId;
	}
	void RemovePawnAndController();
	void TakeMonsterDamage(float dmg);
	virtual void SimulateMovement(float delta_time);

	int32 index = -1;

protected:
	bool GetMovementAllowed();
	virtual void BeginPlay() override;
	
	UPROPERTY()
	UBT_Monster* BehaviorTreeAsset;
	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorComp;
	
	UPROPERTY()
	UBB_Monster* BlackboardAsset;
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComp;
};
