#include "BaseMonsterAIController.h"

#include "DPGameModeBase.h"
#include "FNetLogger.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BehaviorTreeAsset = CreateDefaultSubobject<UBT_Monster>(TEXT("BehaviorTreeAsset"));
	BlackboardAsset = CreateDefaultSubobject<UBB_Monster>(TEXT("BlackboardAsset"));
}

void ABaseMonsterAIController::RemovePawnAndController()
{
	StopMovement();
	
	ABaseMonsterCharacter* ControlledCharacter = Cast<ABaseMonsterCharacter>(GetCharacter());
	if (ControlledCharacter)
	{
		int32 id = Cast<ABaseMonsterCharacter>(GetCharacter())->MonsterId;
		FNetLogger::LogInfo(TEXT("Monster destroyed. - Character exsist, in Controller id: %d"), id);
		ControlledCharacter->Destroy();
	}

	ADPGameModeBase* GM = GetWorld()->GetAuthGameMode<ADPGameModeBase>();
	if (GM)
	{
		GM->empty_monster_slots_.push_back(index);
		GM->monster_controllers_[index] = nullptr;
	}
	Destroy();
}

void ABaseMonsterAIController::TakeMonsterDamage(float dmg)
{
	ABaseMonsterCharacter* CC = Cast<ABaseMonsterCharacter>(GetCharacter());
	check(CC);
	CC->TakeMonsterDamage(dmg);
}

bool ABaseMonsterAIController::GetMovementAllowed()
{
	ABaseMonsterCharacter* CC = Cast<ABaseMonsterCharacter>(GetCharacter());
	check(CC);
	if (CC->GetState() == EMonsterState::Faint)
	{
		FPathFollowingResultFlags::Type AbortFlags = FPathFollowingResultFlags::UserAbort;
		FAIRequestID RequestID = GetCurrentMoveRequestID();
		EPathFollowingVelocityMode VelocityMode = EPathFollowingVelocityMode::Keep;
		GetPathFollowingComponent()->AbortMove(*this, AbortFlags, RequestID, VelocityMode);
		return false;
	}
	return true;
}
