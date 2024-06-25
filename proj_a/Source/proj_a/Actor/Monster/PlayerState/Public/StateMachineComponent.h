#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachineComponent.generated.h"

UENUM()
enum class ECharacterState : uint8
{
	Idle,
	Moving,
	Jumping,
	Attacking,
	Stunned
};

USTRUCT()
struct FCharacterStateData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsMoving = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, ECharacterState, NewState);

UCLASS()
class PROJ_A_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()
public:    
	UStateMachineComponent();
	
protected:
	virtual void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

public:
	UPROPERTY()
	ECharacterState CurrentState;
	UPROPERTY()
	FCharacterStateData StateData;

	UFUNCTION()
	void SetState(ECharacterState NewState);

	UPROPERTY()
	FOnStateChanged OnStateChanged;

private:
	UFUNCTION()
	void HandleCurrentState();

	UFUNCTION()
	void HandleEnemyDetection(AActor* DetectedEnemy);
	
	TMap<ECharacterState, TFunction<void()>> StateMap;
	
	void HandleIdleState();
	void HandleMovingState();
	void HandleJumpingState();
	void HandleStunnedState();
	
	void InitializeStateMap();
};
