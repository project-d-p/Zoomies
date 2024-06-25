#include "StateMachineComponent.h"

#include "AISightComponent.h"
#include "BaseMonsterAIController.h"
#include "BaseMonsterPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"

UStateMachineComponent::UStateMachineComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentState = ECharacterState::Idle;
    InitializeStateMap();
}

void UStateMachineComponent::BeginPlay()
{
    Super::BeginPlay();

    ABaseMonsterAIController* AICon = Cast<ABaseMonsterAIController>(Cast<APlayerState>(GetOwner())->GetOwningController());
    if (AICon)
    {
        AICon->GetAISightComponent()->OnEnemyDetected.AddDynamic(this, &UStateMachineComponent::HandleEnemyDetection);
    }
}

void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    HandleCurrentState();
}

void UStateMachineComponent::SetState(ECharacterState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnStateChanged.Broadcast(NewState);
    }
}

void UStateMachineComponent::HandleCurrentState()
{
    StateMap[CurrentState]();
}

void UStateMachineComponent::HandleEnemyDetection(AActor* DetectedEnemy)
{
    // set Attacking state
}

void UStateMachineComponent::HandleIdleState()
{
    if (StateData.bIsMoving)
    {
        SetState(ECharacterState::Moving);
    }
    else if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        if (OwnerCharacter->GetCharacterMovement()->IsFalling())
        {
            SetState(ECharacterState::Jumping);
        }
    }
}

void UStateMachineComponent::HandleMovingState()
{
    if (!StateData.bIsMoving)
    {
        SetState(ECharacterState::Idle);
    }
    else if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        if (OwnerCharacter->GetCharacterMovement()->IsFalling())
        {
            SetState(ECharacterState::Jumping);
        }
    }
}

void UStateMachineComponent::HandleJumpingState()
{
    if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        if (!OwnerCharacter->GetCharacterMovement()->IsFalling())
        {
            SetState(StateData.bIsMoving ? ECharacterState::Moving : ECharacterState::Idle);
        }
    }
}

void UStateMachineComponent::HandleStunnedState()
{
    
}

void UStateMachineComponent::InitializeStateMap()
{
    StateMap.Add(ECharacterState::Idle, [this]() { HandleIdleState(); });
    StateMap.Add(ECharacterState::Moving, [this]() { HandleMovingState(); });
    StateMap.Add(ECharacterState::Jumping, [this]() { HandleJumpingState(); });
    StateMap.Add(ECharacterState::Stunned, [this]() { HandleStunnedState(); });
}
