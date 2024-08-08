#include "ChasePlayerMonsterAIController.h"

#include "DPGameModeBase.h"
#include "FNetLogger.h"
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
	
	SetRandomPlayerIndex();
}

void AChasePlayerMonsterAIController::SetRandomPlayerIndex()
{
	int32 NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	int32 NewIndex = FMath::RandRange(0, NumPlayers - 1);

	FNetLogger::EditerLog(FColor::Green, TEXT("New Player Index: %d"), NewIndex);
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
	if (!GetMovementAllowed())
		return;
	MoveToPlayer();
}
