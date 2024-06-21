#include "CharacterPositionSync.h"

#include "DPPlayerState.h"
#include "FDataHub.h"
#include "DPCharacter.h"
#include "FNetLogger.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Serialization/BulkDataRegistry.h"

UCharacterPositionSync::UCharacterPositionSync()
{
}

UCharacterPositionSync::~UCharacterPositionSync()
{
}

void UCharacterPositionSync::SyncWithServer(ADPCharacter* character)
{
	player_state_ = Cast<ADPPlayerState>(character->GetPlayerState());
	if (player_state_ == nullptr)
	{
		return;
	}
	
	const FString PlayerId = player_state_->GetPlayerName();
	if (!FDataHub::actorPosition.Contains(PlayerId) && !FDataHub::jumpData.Contains(PlayerId))
	{
		return;
	}
	actor_position_ = FDataHub::actorPosition[PlayerId];

	this->SetState(character);
	
	SyncOrientation(character);
	SyncPosition(character);
}

void UCharacterPositionSync::SyncMyself(ADPCharacter* character)
{
	player_state_ = Cast<ADPPlayerState>(character->GetPlayerState());
	const FString PlayerId = player_state_->GetPlayerName();
	
	if (character->GetMovementComponent()->IsFalling())
	{
		return;
	}
	actor_position_ = FDataHub::actorPosition[PlayerId];
	
	FVector position = FVector(actor_position_.position().x(), actor_position_.position().y(), actor_position_.position().z());
	FVector velocity = FVector(actor_position_.velocity().x(), actor_position_.velocity().y(), actor_position_.velocity().z());
	
	FVector current_position = character->GetActorLocation();
	FVector curren_velocity = character->GetCharacterMovement()->Velocity;
	
	float significantDifference = 50.0f;
	float distance = FVector::Dist(position, current_position);
	
	if (distance > significantDifference)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Distance is too far, Syncing position."));
		this->SyncPosition(character);
	}
}

void UCharacterPositionSync::SetState(ADPCharacter* character)
{
	State state = actor_position_.state();
	switch (state)
	{
	case State::STATE_Walking:
		character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		break;
	case State::STATE_Falling:
		character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		break;
	default:
		character->GetCharacterMovement()->SetMovementMode(MOVE_None);
		break;
	}
}

void UCharacterPositionSync::SyncPosition(ADPCharacter* character)
{
	FVector position = FVector(actor_position_.position().x(), actor_position_.position().y(), actor_position_.position().z());
	FVector velocity = FVector(actor_position_.velocity().x(), actor_position_.velocity().y(), actor_position_.velocity().z());
	
	FVector current_position = character->GetActorLocation();
	FVector curren_velocity = character->GetCharacterMovement()->Velocity;
	
	FVector interpolated_position = FMath::VInterpTo(current_position, position, GetWorld()->GetDeltaSeconds(), 10.0f);
	FVector interpolated_velocity = FMath::VInterpTo(curren_velocity, velocity, GetWorld()->GetDeltaSeconds(), 10.0f);
	
	character->SetActorLocation(interpolated_position);
	character->GetCharacterMovement()->Velocity = interpolated_velocity;
}

void UCharacterPositionSync::SyncOrientation(ADPCharacter* character)
{
	FVector velocity = FVector(actor_position_.velocity().x(), actor_position_.velocity().y(), actor_position_.velocity().z());

	if (velocity.X == 0.0f && velocity.Y == 0.0f)
	{
		return ;
	}
	
	FVector current_position = character->GetActorLocation();
	FVector curren_velocity = character->GetCharacterMovement()->Velocity;
	
	FVector UnitVector = velocity.GetSafeNormal();
	FVector LookAtTarget = current_position + UnitVector;
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(current_position, LookAtTarget);
	
	FRotator FinalRotation = FRotator(0, NewRotation.Yaw, 0);
		
	character->SetActorRotation(FinalRotation);
}

void UCharacterPositionSync::SyncJump(ADPCharacter* character)
{
	// character->StopJumping();
	character->Jump();
	// UGameplayStatics::PlaySound2D(GetWorld(), jumpSound);
}



