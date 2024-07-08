#include "ChasePlayerMonsterAIController.h"

#include "DPGameModeBase.h"
#include "GameHelper.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

void AChasePlayerMonsterAIController::MoveToPlayer()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), PlayerIndex);

	if (PlayerCharacter)
	{
		float AcceptanceRadius = 50.0f;
		bool bStopOnOverlap = true;
		bool bUsePathfinding = true;
		bool bCanStrafe = false;
		TSubclassOf<UNavigationQueryFilter> FilterClass = nullptr;
		bool bAllowPartialPath = true;
		
		MoveToActor(
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
	
	int32 NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	PlayerIndex = FMath::RandRange(0, NumPlayers - 1);
}

void AChasePlayerMonsterAIController::SimulateMovement(float delta_time)
{
	if (!GetMovementAllowed())
		return;
	MoveToPlayer();
}
