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
	: b_is_jumping_(false)
{
}

UCharacterPositionSync::~UCharacterPositionSync()
{
}

void UCharacterPositionSync::SyncWithServer(ADPCharacter* character)
{
	if (character->GetCharacterMovement()->IsJumpAllowed())
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Allow Jumping."));
	}
	// character->GetCharacterMovement()->DoJump(true);
	character->GetCharacterMovement()->AddImpulse(FVector(0, 0, 1000));
	return ;
	// player_state_ = Cast<ADPPlayerState>(character->GetPlayerState());
	// if (player_state_ == nullptr)
	// {
	// 	return;
	// }
	//
	// const FString PlayerId = player_state_->GetPlayerName();
	// if (!FDataHub::actorPosition.Contains(PlayerId) && !FDataHub::jumpData.Contains(PlayerId))
	// {
	// 	return;
	// }
	// actor_position_ = FDataHub::actorPosition[PlayerId];
	//
	// if (FDataHub::jumpData.Contains(PlayerId))
	// {
	// 	last_jump_time = GetWorld()->GetTimeSeconds();
	// 	b_is_jumping_ = true;
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Jumping."));
	// 	jump_ = FDataHub::jumpData[PlayerId];
	// 	FDataHub::jumpData.Remove(PlayerId);
	// 	SyncJump(character);
	// }
	// else if (b_is_jumping_ && GetWorld()->GetTimeSeconds() - last_jump_time < 0.5f)
	// {
	// 	return ;
	// }
	// else if (character->GetMovementComponent()->IsFalling())
	// {
	// 	return;
	// }
	// else
	// {
	// 	b_is_jumping_ = false;
	// 	SyncOrientation(character);
	// 	SyncPosition(character);
	// }
}

void UCharacterPositionSync::SyncMyself(ADPCharacter* character)
{
	if (character->CanJump())
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("OWN Jumping."));
	}
	else
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("OWN Not Jumping."));
	}
	player_state_ = Cast<ADPPlayerState>(character->GetPlayerState());
	const FString PlayerId = player_state_->GetPlayerName();
	
	if (character->GetMovementComponent()->IsFalling())
	{
		return;
	}
	
	if (!FDataHub::actorPosition.Contains(PlayerId))
	{
		return;
	}
	actor_position_ = FDataHub::actorPosition[PlayerId];
	
	FVector position = FVector(actor_position_.position().x(), actor_position_.position().y(), actor_position_.position().z());
	FVector velocity = FVector(actor_position_.velocity().x(), actor_position_.velocity().y(), actor_position_.velocity().z());
	
	FVector current_position = character->GetActorLocation();
	FVector curren_velocity = character->GetCharacterMovement()->Velocity;
	
	float significantDifference = 100.0f;
	float distance = FVector::Dist(position, current_position);
	
	if (distance > significantDifference)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Distance is too far, Syncing position."));
		this->SyncPosition(character);
	}
}

void UCharacterPositionSync::SyncPosition(ADPCharacter* character)
{
	FVector position = FVector(actor_position_.position().x(), actor_position_.position().y(), actor_position_.position().z());
	FVector velocity = FVector(actor_position_.velocity().x(), actor_position_.velocity().y(), actor_position_.velocity().z());
	
	// if (velocity == FVector(0, 0, 0))
	// {
	// 	character->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
	// 	return ;
	// }
	
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

	if (velocity == FVector(0, 0, 0))
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



