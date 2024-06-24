#include "BaseMonsterAIController.h"

#include "AISightComponent.h"
#include "BaseMonsterPlayerState.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	AISightComponent = CreateDefaultSubobject<UAISightComponent>(TEXT("AISight"));
	bWantsPlayerState = true;
}

UAISightComponent* ABaseMonsterAIController::GetAISightComponent() const
{
	return AISightComponent;
}

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	// PlayerState 캐스팅 및 할당
	if (GetPawn()) // Pawn이 존재하는지 확인
	{
		ABaseMonsterPlayerState* MonsterPlayerState = Cast<ABaseMonsterPlayerState>(GetPawn()->GetPlayerState());
		if (MonsterPlayerState)
		{
			PlayerState = MonsterPlayerState;
			// 추가적인 초기화나 설정을 여기에서 수행
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerState is not of type ABaseMonsterPlayerState"));
		}
	}
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