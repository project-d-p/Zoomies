#include "TargetPointMonsterAIController.h"

#include "DPGameModeBase.h"
#include "GameHelper.h"
#include "Navigation/PathFollowingComponent.h"

void ATargetPointMonsterAIController::MoveToTargetLocation()
{
	static FVector Dest = FVector(4000.0f, 0.0f, 0.0f);
	float AcceptanceRadius = 50.0f;
	bool bStopOnOverlap = true;
	bool bUsePathfinding = true;
	bool bProjectDestinationToNavigation = true;
	bool bCanStrafe = false;
	TSubclassOf<UNavigationQueryFilter> FilterClass = nullptr;
	bool bAllowPartialPath = true;
	
	EPathFollowingRequestResult::Type Result = MoveToLocation(Dest, AcceptanceRadius, bStopOnOverlap, bUsePathfinding,
															  bProjectDestinationToNavigation, bCanStrafe, FilterClass, bAllowPartialPath);
	
	if (Result == EPathFollowingRequestResult::Type::AlreadyAtGoal)
	{
		RemovePawnAndController();
	}
}

void ATargetPointMonsterAIController::SimulateMovement(float delta_time)
{
	if (!GetMovementAllowed())
		return;
	MoveToTargetLocation();
}
