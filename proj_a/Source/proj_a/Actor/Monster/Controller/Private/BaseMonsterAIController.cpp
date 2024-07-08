#include "BaseMonsterAIController.h"

#include "DPGameModeBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

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

void ABaseMonsterAIController::TakeDamage(float dmg)
{
	ABaseMonsterCharacter* CC = Cast<ABaseMonsterCharacter>(GetCharacter());
	check(CC);
	CC->TakeDamage(dmg);
}

bool ABaseMonsterAIController::GetMovementAllowed()
{
	ABaseMonsterCharacter* CC = Cast<ABaseMonsterCharacter>(GetCharacter());
	check(CC);
	if (CC->GetState() == Faint)
		return false;
	return true;
}
