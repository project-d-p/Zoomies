#include "CharacterPositionSync.h"

#include "BaseMonsterCharacter.h"
#include "DPPlayerState.h"
#include "FDataHub.h"
#include "DPCharacter.h"
#include "DPWeaponActorComponent.h"
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

void UCharacterPositionSync::PlayAimAnimation(ADPCharacter* character)
{
	Player_State = Cast<ADPPlayerState>(character->GetPlayerState());
	if (Player_State == nullptr)
	{
		return;
	}
	const FString PlayerId = Player_State->GetPlayerName();
	AimState* aim_state = FDataHub::aimStateData.Find(PlayerId);
	if (aim_state == nullptr)
	{
		return;
	}
	FDataHub::aimStateData.Remove(PlayerId);
	switch (aim_state->aim_state())
	{
	case AimState::AIM_STATE_ACTIVE:
		character->PlayAimAnimation();
		break;
	case AimState::AIM_STATE_RELEASE:
		character->StopAimAnimation();
		break;
	default:
		break;
	}
}

void UCharacterPositionSync::SyncWithServer(ADPCharacter* character)
{
	Player_State = Cast<ADPPlayerState>(character->GetPlayerState());
	if (Player_State == nullptr)
	{
		return;
	}
	
	const FString PlayerId = Player_State->GetPlayerName();

	ActorPosition* actor_position = FDataHub::actorPosition.Find(PlayerId);
	if (actor_position == nullptr)
	{
		return;
	}
	
	actor_position_ = *actor_position;

	this->SetState(character);

	this->PlayAimAnimation(character);

	SyncOrientation(character);
	SyncPosition(character);
}

void UCharacterPositionSync::SyncGunFire(ADPCharacter* character)
{
	Player_State = Cast<ADPPlayerState>(character->GetPlayerState());
	if (Player_State == nullptr)
	{
		return;
	}
	const FString PlayerId = Player_State->GetPlayerName();
	Gunfire* gunfire = FDataHub::gunfireData.Find(PlayerId);
	if (gunfire == nullptr)
	{
		return;
	}
	gunfire_ = *gunfire;
	FDataHub::gunfireData.Remove(PlayerId);
	FHitResult hit_result;
	
	if (character->weaponComponent->SimulateAttack(character, hit_result, gunfire_))
	{
	}
}

void UCharacterPositionSync::SyncCatch(ADPCharacter* character)
{
	Player_State = Cast<ADPPlayerState>(character->GetPlayerState());
	if (Player_State == nullptr)
	{
		return;
	}
	const FString PlayerId = Player_State->GetPlayerName();
	Catch* catch_ = FDataHub::catchData.Find(PlayerId);
	if (catch_ == nullptr)
	{
		return;
	}
	Catch catch_data = *catch_;
	FDataHub::catchData.Remove(PlayerId);
	FString monster_id = UTF8_TO_TCHAR(catch_data.target().c_str());
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Catch monster_id: %s"), *monster_id);
	character->CatchMonster(monster_id);
}

void UCharacterPositionSync::SyncReturnAnimal(ADPCharacter* character)
{
	Player_State = Cast<ADPPlayerState>(character->GetPlayerState());
	if (Player_State == nullptr)
	{
		return;
	}
	const FString PlayerId = Player_State->GetPlayerName();
	bool* isReturn = FDataHub::returnAnimalData.Find(PlayerId);
	if (isReturn == nullptr)
	{
		return;
	}
	if (*isReturn)
	{
		character->ReturnMonsters();
	}
	FDataHub::returnAnimalData.Remove(PlayerId);
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
	FVector current_velocity = character->GetCharacterMovement()->Velocity;
	
	FVector interpolated_position = FMath::VInterpTo(current_position, position, GetWorld()->GetDeltaSeconds(), 10.0f);
	FVector interpolated_velocity = FMath::VInterpTo(current_velocity, velocity, GetWorld()->GetDeltaSeconds(), 10.0f);
	
	character->SetActorLocation(interpolated_position);
	character->GetCharacterMovement()->Velocity = interpolated_velocity;
}

void UCharacterPositionSync::SyncOrientationWithRotation(ADPCharacter* character)
{
	FRotator rotation = FRotator(actor_position_.rotation().x(), actor_position_.rotation().y(), actor_position_.rotation().z());
	FRotator final_rotation = FRotator(0, rotation.Yaw, 0);
	character->SetActorRotation(final_rotation);
}

void UCharacterPositionSync::SyncOrientationWithController(ADPCharacter* character)
{
	FRotator control_rotation = FRotator(actor_position_.control_rotation().x(), actor_position_.control_rotation().y(), actor_position_.control_rotation().z());
	FRotator final_rotation = FRotator(0, control_rotation.Yaw, 0);
	character->SetActorRotation(final_rotation);
}

void UCharacterPositionSync::SyncOrientation(ADPCharacter* character)
{
	if (character->isAim == true)
	{
		this->SyncOrientationWithController(character);
	}
	else
	{
		this->SyncOrientationWithRotation(character);
	}
}

void UCharacterPositionSync::SyncJump(ADPCharacter* character)
{
	// character->StopJumping();
	character->Jump();
	// UGameplayStatics::PlaySound2D(GetWorld(), jumpSound);
}



