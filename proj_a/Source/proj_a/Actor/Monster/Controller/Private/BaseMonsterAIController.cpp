#include "BaseMonsterAIController.h"

#include "DPGameModeBase.h"
#include "FDataHub.h"
#include "GameHelper.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdManager.h"
#include "Navigation/PathFollowingComponent.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	
	// UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this);
	// if (CrowdManager)
	// {j
	// 	CrowdManager->RegisterAgent(this);
	// }

	BehaviorTreeAsset = CreateDefaultSubobject<UBT_Monster>(TEXT("BehaviorTreeAsset"));
	BlackboardAsset = CreateDefaultSubobject<UBB_Monster>(TEXT("BlackboardAsset"));
}

void ABaseMonsterAIController::RemovePawnAndController()
{
	StopMovement();
	
	// UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this);
	// if (CrowdManager)
	// {
	// 	CrowdManager->UnregisterAgent(this);
	// }

	int32 index = Cast<ABaseMonsterCharacter>(GetCharacter())->index;
	int32 id = Cast<ABaseMonsterCharacter>(GetCharacter())->MonsterId;
	// XXX: 이거 서버가 제거하는게 아니라 클라이언트가 제거하도록 해야한다.

	ADPGameModeBase* GM = GetWorld()->GetAuthGameMode<ADPGameModeBase>();
	if (GM)
	{
		GM->monster_controllers_[index] = nullptr;
		// GM->empty_monster_slots_.push_back(index);
	}
	
	ABaseMonsterCharacter* ControlledCharacter = Cast<ABaseMonsterCharacter>(GetCharacter());
	if (ControlledCharacter)
	{
		ControlledCharacter->Destroy();
	}
	Destroy();
}