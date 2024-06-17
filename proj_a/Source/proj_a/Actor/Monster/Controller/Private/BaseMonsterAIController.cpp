#include "BaseMonsterAIController.h"

#include "AISightComponent.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	AISightComponent = CreateDefaultSubobject<UAISightComponent>(TEXT("AISight"));
	bWantsPlayerState = true;
}

UAISightComponent* ABaseMonsterAIController::GetAISightComponent() const
{
	return AISightComponent;
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