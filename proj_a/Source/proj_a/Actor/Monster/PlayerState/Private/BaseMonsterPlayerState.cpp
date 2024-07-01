#include "BaseMonsterPlayerState.h"

ABaseMonsterPlayerState::ABaseMonsterPlayerState()
{
	CurrentHealth = 100.0f;
	
	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));
	OnTakeAnyDamage.AddDynamic(this, &ABaseMonsterPlayerState::OnTakeDamage);

	// TODO : UniqueID를 생성하고 PlayerState에 할당
}

float ABaseMonsterPlayerState::GetCurrentHealth() const
{
	return CurrentHealth;
}

ECharacterState ABaseMonsterPlayerState::GetCurrentState() const
{
	return StateMachineComponent->CurrentState;
}

void ABaseMonsterPlayerState::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth -= Damage;
	if (CurrentHealth <= 0.0f)
	{
		// OnCharacterDeath.Broadcast(this);
	}
}
