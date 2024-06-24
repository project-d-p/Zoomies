#include "BaseMonsterAIController.h"

#include "BaseMonsterPlayerState.h"
#include "Navigation/PathFollowingComponent.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	// AISightComponent = CreateDefaultSubobject<UAISightComponent>(TEXT("AISight"));
	bWantsPlayerState = true;
	PlayerState = Cast<ABaseMonsterPlayerState>(PlayerState);
}

UAISightComponent* ABaseMonsterAIController::GetAISightComponent() const
{
	return AISightComponent;
}

void ABaseMonsterAIController::MoveToTargetLocation()
{
	static FVector Dest = FVector(1500.0f, 0.0f, 0.0f);
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
		Dest = Dest == FVector(1500.0f, 0.0f, 0.0f) ? FVector(-1500.0f, 0.0f, 0.0f) : FVector(1500.0f, 0.0f, 0.0f);
	}
	else if (Result == EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		// FNetLogger::EditerLog(FColor::Green, TEXT("AI move request successful."));
	}
}

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	MoveToTargetLocation();
}

void ABaseMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveToTargetLocation();
}

// void ABaseMonsterAIController::OnPossess(APawn* InPawn)
// {
// 	Super::OnPossess(InPawn);
//
// 	// Custom behavior when AI possesses a pawn
// }
//
// void ABaseMonsterAIController::OnUnPossess()
// {
// 	Super::OnUnPossess();
//
// 	// Custom behavior when AI unpossesses a pawn
// }