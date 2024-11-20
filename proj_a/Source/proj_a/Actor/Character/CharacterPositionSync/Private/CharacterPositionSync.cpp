#include "CharacterPositionSync.h"

#include "BaseMonsterCharacter.h"
#include "DPPlayerState.h"
#include "FDataHub.h"
#include "DPCharacter.h"
#include "DPWeaponActorComponent.h"
#include "FNetLogger.h"
#include "ReturnTriggerVolume.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CompileMode.h"
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
#if EDITOR_MODE || LAN_MODE
	const FString PlayerId = Player_State->GetPlayerName();
#else
	const FString PlayerId = Player_State->GetUniqueId()->ToString();
#endif
	TOptional<AimState> AimStateOpt = FDataHub::GetAimStateData(PlayerId);
	if (!AimStateOpt.IsSet())
	{
		return;
	}
	AimState AimStateData = AimStateOpt.GetValue();
	
	switch (AimStateData.aim_state())
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
	
#if EDITOR_MODE || LAN_MODE
	const FString PlayerId = Player_State->GetPlayerName();
#else
	const FString PlayerId = Player_State->GetUniqueId()->ToString();
#endif

	// ActorPosition* actor_position = FDataHub::actorPosition.Find(PlayerId);
	// if (actor_position == nullptr)
	// {
	// 	return;
	// }

	TOptional<ActorPosition> ActorPositionOpt = FDataHub::GetActorPositionData(PlayerId);
	if (!ActorPositionOpt.IsSet())
	{
		return;
	}
	
	actor_position_ = ActorPositionOpt.GetValue();

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
#if EDITOR_MODE || LAN_MODE
	const FString PlayerId = Player_State->GetPlayerName();
#else
	const FString PlayerId = Player_State->GetUniqueId()->ToString();
#endif

	TOptional<Gunfire> GunfireOpt = FDataHub::GetGunfireData(PlayerId);
	if (!GunfireOpt.IsSet())
	{
		return;
	}
	gunfire_ = GunfireOpt.GetValue();

	FHitResult hit_result;
	
	if (character->weaponComponent->SimulateAttack(character, hit_result, gunfire_))
	{
	}

	FRotator direction = FRotator(gunfire_.direction().x(), gunfire_.direction().y(), gunfire_.direction().z());
	character->weaponComponent->SpawnEffects(hit_result, direction);
}

void UCharacterPositionSync::SyncCatch(ADPCharacter* character)
{
	Player_State = Cast<ADPPlayerState>(character->GetPlayerState());
	if (Player_State == nullptr)
	{
		return;
	}
#if EDITOR_MODE || LAN_MODE
	const FString PlayerId = Player_State->GetPlayerName();
#else
	const FString PlayerId = Player_State->GetUniqueId()->ToString();
#endif

	TOptional<Catch> CatchOpt = FDataHub::GetCatchData(PlayerId);
	if (!CatchOpt.IsSet())
	{
		return;
	}
	Catch CatchData = CatchOpt.GetValue();
	
	FString monster_id = UTF8_TO_TCHAR(CatchData.target().c_str());
	character->CatchMonster(monster_id);
}

void UCharacterPositionSync::SyncReturnAnimal(ADPCharacter* character)
{
	Player_State = Cast<ADPPlayerState>(character->GetPlayerState());
	if (Player_State == nullptr)
	{
		return;
	}
#if EDITOR_MODE || LAN_MODE
	const FString PlayerId = Player_State->GetPlayerName();
#else
	const FString PlayerId = Player_State->GetUniqueId()->ToString();
#endif
	TOptional<bool> ReturnAnimalOpt = FDataHub::GetReturnAnimalData(PlayerId);
	if (!ReturnAnimalOpt.IsSet())
	{
		return;
	}
	if (ReturnAnimalOpt.GetValue())
	{
		TArray<EAnimal> animals = character->ReturnMonsters();
		Player_State->IncreaseScore(animals);
		if (character->ReturnTriggerVolume)
		{
			character->ReturnTriggerVolume->SpawnReturnEffect(animals);
		}
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



