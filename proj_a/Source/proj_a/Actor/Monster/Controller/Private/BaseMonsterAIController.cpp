#include "BaseMonsterAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdManager.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	
	bWantsPlayerState = true;

	UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this);
	if (CrowdManager)
	{
		CrowdManager->RegisterAgent(this);
	}
	// XXX: When the Monster AI is deleted, unregister the agent.

	BehaviorTreeAsset = CreateDefaultSubobject<UBT_Monster>(TEXT("BehaviorTreeAsset"));
	BlackboardAsset = CreateDefaultSubobject<UBB_Monster>(TEXT("BlackboardAsset"));
}

void ABaseMonsterAIController::RemovePawnAndController()
{
	UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this);
	if (CrowdManager)
	{
		CrowdManager->UnregisterAgent(this);
	}
	
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		ControlledPawn->Destroy();
	}

	Destroy();
}