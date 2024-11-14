#include "ChasePlayerMonsterAIController.h"

#include "DPGameModeBase.h"
#include "FNetLogger.h"
#include "GameHelper.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"


void AChasePlayerMonsterAIController::MoveToPlayer()
{
	if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), PlayerIndex))
	{
		float AcceptanceRadius = 100.0f;
		bool bStopOnOverlap = true;
		bool bUsePathfinding = true;
		bool bCanStrafe = false;
		TSubclassOf<UNavigationQueryFilter> FilterClass = nullptr;
		bool bAllowPartialPath = true;

		EPathFollowingRequestResult::Type MoveResult = MoveToActor(
		PlayerCharacter,
		AcceptanceRadius,
		bStopOnOverlap,
		bUsePathfinding,
		bCanStrafe,
		FilterClass,
		bAllowPartialPath);
	}
}

void AChasePlayerMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	
	SetRandomPlayerIndex();
}

void AChasePlayerMonsterAIController::SetRandomPlayerIndex()
{
	int32 NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	int32 NewIndex = FMath::RandRange(0, NumPlayers - 1);

	/* Set index to 0 if no controllers exist in the world, preventing engine errors */
	if (NumPlayers <= 1)
	{
		PlayerIndex = 0;
		return;
	}
	
	if (NewIndex == PlayerIndex && PlayerIndex != -1)
	{
		NewIndex = (NewIndex + 1) % NumPlayers;
	}

	PlayerIndex = NewIndex;
}

void AChasePlayerMonsterAIController::SimulateMovement(float delta_time)
{
	Super::SimulateMovement(delta_time);
	if (!GetMovementAllowed())
	{
		return;
	}
	MoveToPlayer();
}
