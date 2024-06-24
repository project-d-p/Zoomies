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

	// PlayerState ĳ���� �� �Ҵ�
	if (GetPawn()) // Pawn�� �����ϴ��� Ȯ��
	{
		ABaseMonsterPlayerState* MonsterPlayerState = Cast<ABaseMonsterPlayerState>(GetPawn()->GetPlayerState());
		if (MonsterPlayerState)
		{
			PlayerState = MonsterPlayerState;
			// �߰����� �ʱ�ȭ�� ������ ���⿡�� ����
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