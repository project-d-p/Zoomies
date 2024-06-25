#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "StateMachineComponent.h"
#include "BaseMonsterPlayerState.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterStunnedDelegate, APlayerState*, PlayerState);

UCLASS()
class PROJ_A_API ABaseMonsterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ABaseMonsterPlayerState();

	UFUNCTION()
	float GetCurrentHealth() const;
	UFUNCTION()
	ECharacterState GetCurrentState() const;
	// UFUNCTION()
	// FOnCharacterStunnedDelegate OnCharacterStunned;
private:
	UPROPERTY()
	float CurrentHealth = 100.0f;
	UPROPERTY()
	UStateMachineComponent* StateMachineComponent = nullptr;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
					  AActor* DamageCauser);
};