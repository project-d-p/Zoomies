#include "BaseMonsterAIController.h"

#include "DPGameModeBase.h"
#include "FNetLogger.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/CrowdManager.h"
#include "Navigation/PathFollowingComponent.h"

ABaseMonsterAIController::ABaseMonsterAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BehaviorTreeAsset = CreateDefaultSubobject<UBT_Monster>(TEXT("BehaviorTreeAsset"));
	BlackboardAsset = CreateDefaultSubobject<UBB_Monster>(TEXT("BlackboardAsset"));
}

void ABaseMonsterAIController::RemovePawnAndController()
{
	if (GetPathFollowingComponent())
	{
		StopMovement();
	}
	else
	{
		FNetLogger::LogError(TEXT("PathFollowingComponent is nullptr"));
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		FNetLogger::LogError(TEXT("World is nullptr"));
		return;
	}
	ADPGameModeBase* GM = GetWorld()->GetAuthGameMode<ADPGameModeBase>();
	if (GM == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to get GameMode"));
		return;
	}

	ABaseMonsterCharacter* ControlledCharacter = Cast<ABaseMonsterCharacter>(GetCharacter());
	if (ControlledCharacter)
	{
		GM->RemoveMonsterData(ControlledCharacter);
		int32 id = ControlledCharacter->MonsterId;
		FNetLogger::LogInfo(TEXT("Monster destroyed. - Character exists, in Controller id: %d"), id);
		ControlledCharacter->Destroy();
	}
	else
	{
		FNetLogger::LogInfo(TEXT("Monster destroyed. - Character does not exist, in Controller id: %d"), GetUniqueID());
		return;
	}

	if (index < 0 || index >= GM->empty_monster_slots_.size())
	{
		FNetLogger::LogError(TEXT("Index out of bounds: %d"), index);
		return;
	}

	GM->empty_monster_slots_.push_back(index);
	if (GM->monster_controllers_.size() > index)
	{
		GM->monster_controllers_[index] = nullptr;
	}
	Destroy();
}

void ABaseMonsterAIController::TakeMonsterDamage(float dmg)
{
	ABaseMonsterCharacter* CC = Cast<ABaseMonsterCharacter>(GetCharacter());
	if (!CC)
	{
		FNetLogger::LogError(TEXT("Failed to cast to ABaseMonsterCharacter Id %d"), GetUniqueID());
		return ;
	}
	CC->TakeMonsterDamage(dmg);
	UCrowdFollowingComponent* CrowdFollowingComp = FindComponentByClass<UCrowdFollowingComponent>();
	if (CrowdFollowingComp)
	{
		UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(GetWorld());
		if (CrowdManager)
		{
			CrowdManager->UnregisterAgent(CrowdFollowingComp);
		}
		CrowdFollowingComp->Deactivate();
	}
	// if (CC->GetCurrentHealth() <= 0)
	constexpr float TIME_TO_REMOVE = 7.f;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseMonsterAIController::RemovePawnAndController, TIME_TO_REMOVE, false);
}

void ABaseMonsterAIController::SimulateMovement(float delta_time)
{
	UCrowdFollowingComponent* CrowdFollowingComp = FindComponentByClass<UCrowdFollowingComponent>();
	if (CrowdFollowingComp)
	{
		const float SafeDistance = 300.0f;
		const float CloseDistance = 100.0f;
	
		TArray<AActor*> NearbyMonsters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseMonsterCharacter::StaticClass(), NearbyMonsters);
	
		for (AActor* NearbyMonster : NearbyMonsters)
		{
			if (NearbyMonster != GetPawn())
			{
				float Distance = FVector::Dist(GetPawn()->GetActorLocation(), NearbyMonster ->GetActorLocation());
	
				if (Distance < CloseDistance)
				{
					CrowdFollowingComp->SetCrowdSeparationWeight(100.0f);
				}
				else if (Distance > SafeDistance)
				{
					CrowdFollowingComp->SetCrowdSeparationWeight(50.0f);
				}
			}
		}
	}
}

bool ABaseMonsterAIController::GetMovementAllowed()
{
	ABaseMonsterCharacter* CC = Cast<ABaseMonsterCharacter>(GetCharacter());
	if (!CC)
	{
		FNetLogger::LogError(TEXT("Failed to cast to ABaseMonsterCharacter ID: %d"), GetUniqueID());
		return false;
	}
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

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	UCrowdFollowingComponent* CrowdFollowingComp = FindComponentByClass<UCrowdFollowingComponent>();
	if (CrowdFollowingComp)
	{
		CrowdFollowingComp->SetCrowdSeparation(true);
		CrowdFollowingComp->SetCrowdSeparationWeight(50.f);
		CrowdFollowingComp->SetCrowdAvoidanceRangeMultiplier(1.2f);
		CrowdFollowingComp->SetCrowdObstacleAvoidance(true);
	}
}
