#include "AvoidPlayerMonsterAIController.h"

#include "DPGameModeBase.h"
#include "FNetLogger.h"
#include "GameHelper.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogMonsterAI);

AAvoidPlayerMonsterAIController::AAvoidPlayerMonsterAIController()
{
	Query = LoadObject<UEnvQuery>(
		nullptr,
			TEXT("/Game/EQS/EQS_CheckDistanceToPlayer.EQS_CheckDistanceToPlayer"));
	if (!Query)
	{
		UE_LOG(LogMonsterAI, Error, TEXT("Failed to load EQS Query"));
	}
}

bool AAvoidPlayerMonsterAIController::IsMovingToTarget() const
{
	return GetPathFollowingComponent() && GetPathFollowingComponent()->HasValidPath();
}

void AAvoidPlayerMonsterAIController::MoveToRandomTargetLocation()
{
	if (IsMovingToTarget())
	{
		return;
	}
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	FVector Origin = GetPawn()->GetActorLocation();
	float RandomRadius = 3000.0f;

	FNavLocation Dest;
	if (NavSys->GetRandomPointInNavigableRadius(Origin, RandomRadius, Dest))
	{
		Dest.Location.Z = Origin.Z;

		BlackboardComp->SetValueAsVector("SafeLocation", Dest);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find a random navigable location"));
	}
}

void AAvoidPlayerMonsterAIController::RunEQSQuery()
{
	if (Query && IsQueryFinished)
	{
		FEnvQueryRequest QueryRequest(Query, GetPawn());
		IsQueryFinished = false;
		QueryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &AAvoidPlayerMonsterAIController::OnQueryFinished);
	}
}

void AAvoidPlayerMonsterAIController::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (Result->IsSuccessful() && Result->Items.Num() > 0)
	{
		FVector PlayerLocation = Result->GetItemAsLocation(0);
        
		FVector AILocation = GetCharacter()->GetActorLocation();
		FVector AILocation1 = GetCharacter()->GetActorLocation();
        
		PlayerLocation.Z = 0.0f;
		AILocation.Z = 0.0f;

		FVector DirectionAwayFromPlayer = (AILocation - PlayerLocation).GetSafeNormal();
		float DistanceToMoveAway = 1000.0f;
		CachedSafeLocation = AILocation1 + DirectionAwayFromPlayer * DistanceToMoveAway;

		BlackboardComp->SetValueAsVector("SafeLocation", CachedSafeLocation);
	}
	else
	{
		MoveToRandomTargetLocation();
	}
	IsQueryFinished = true;
}

void AAvoidPlayerMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (BehaviorTreeAsset && BlackboardAsset)
	{
		if (!UseBlackboard(BlackboardAsset, BlackboardComp))
			UE_LOG(LogMonsterAI, Error, TEXT("UseBlackboard failed in BeginPlay!"));
		if (!RunBehaviorTree(BehaviorTreeAsset))
			UE_LOG(LogMonsterAI, Error, TEXT("RunBehaviorTree failed in BeginPlay!"));
	}
	else
	{
		UE_LOG(LogMonsterAI, Error, TEXT("BehaviorTreeAsset or BlackboardAsset is null in BeginPlay!"));
	}
}

void AAvoidPlayerMonsterAIController::SimulateMovement(float delta_time)
{
	Super::SimulateMovement(delta_time);
	if (!GetMovementAllowed())
		return;
	elapsed_DeSpawnTime += delta_time;
	if (elapsed_DeSpawnTime >= DeSpawnTime)
	{
		RemovePawnAndController();
		return ;
	}
	elapsed_EQSQueryTime += delta_time;
	if (elapsed_EQSQueryTime >= EQSQueryTime)
	{
		RunEQSQuery();
		elapsed_EQSQueryTime = 0;
	}
}
