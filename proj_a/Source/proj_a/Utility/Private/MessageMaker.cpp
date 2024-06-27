#include "MessageMaker.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "PlayerName.h"
#include "PropertyEditorModule.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerState.h"

Message MessageMaker::MakeMovementMessage(const ADPPlayerController* Controller, const FVector2d& ActionValue, const FRotator& Rotation, const FVector& Velocity)
{
	Message msg;
	if (Controller == nullptr)
	{
		return msg;
	}
	if (Controller->PlayerState == nullptr)
	{
		return msg;
	}
	
	msg.set_player_id(TCHAR_TO_UTF8(*Controller->PlayerState->GetPlayerName()));
	Movement movement;
	Vec3 velocity;
	velocity.set_x(Velocity.X);
	velocity.set_y(Velocity.Y);
	velocity.set_z(Velocity.Z);
	*movement.mutable_velocity() = velocity;
	Vec3 action;
	action.set_x(ActionValue.X);
	action.set_y(ActionValue.Y);
	*movement.mutable_action() = action;
	Vec3 rotation;
	rotation.set_x(Rotation.Pitch);
	rotation.set_y(Rotation.Yaw);
	rotation.set_z(Rotation.Roll);
	*movement.mutable_rotation() = rotation;
	float delta = Controller->GetWorld()->GetDeltaSeconds();
	movement.set_delta_time(delta);
	*msg.mutable_movement() = movement;
	return msg;
}

Message MessageMaker::MakePositionMessage(const ADPPlayerController* Controller)
{
	Message msg;
	if (Controller == nullptr)
	{
		return msg;
	}
	if (Controller->PlayerState == nullptr)
	{
		return msg;
	}
	msg.set_player_id(TCHAR_TO_UTF8(*Controller->PlayerState->GetPlayerName()));
	ActorPosition actor_position;
	Vec3 position;
	position.set_x(Controller->GetPawn()->GetActorLocation().X);
	position.set_y(Controller->GetPawn()->GetActorLocation().Y);
	position.set_z(Controller->GetPawn()->GetActorLocation().Z);
	*actor_position.mutable_position() = position;
	Vec3 rotation;
	rotation.set_x(Controller->GetPawn()->GetActorRotation().Pitch);
	rotation.set_y(Controller->GetPawn()->GetActorRotation().Yaw);
	rotation.set_z(Controller->GetPawn()->GetActorRotation().Roll);
	*actor_position.mutable_rotation() = rotation;
	Vec3 control_rotation;
	control_rotation.set_x(Controller->GetControlRotation().Pitch);
	control_rotation.set_y(Controller->GetControlRotation().Yaw);
	control_rotation.set_z(Controller->GetControlRotation().Roll);
	*actor_position.mutable_control_rotation() = control_rotation;
	Vec3 velocity;
	velocity.set_x(Controller->GetPawn()->GetVelocity().X);
	velocity.set_y(Controller->GetPawn()->GetVelocity().Y);
	velocity.set_z(Controller->GetPawn()->GetVelocity().Z);
	*actor_position.mutable_velocity() = velocity;
	ADPCharacter* character = Cast<ADPCharacter>(Controller->GetPawn());
	switch (character->GetCharacterMovement()->MovementMode)
	{
	case EMovementMode::MOVE_Walking:
		actor_position.set_state(State::STATE_Walking);
		break;
	case EMovementMode::MOVE_Falling:
		actor_position.set_state(State::STATE_Falling);
		break;
	default:
		actor_position.set_state(State::STATE_None);
		break;
	}
	*msg.mutable_actor_position() = actor_position;
	return msg;
}

Message MessageMaker::MakeJumpMessage(ADPPlayerController* Controller)
{
	Message msg;
	if (Controller == nullptr)
	{
		return msg;
	}
	if (Controller->PlayerState == nullptr)
	{
		return msg;
	}

	msg.set_player_id(TCHAR_TO_UTF8(*Controller->PlayerState->GetPlayerName()));
	Jump jump;
	jump.set_jump_event(1);
	*msg.mutable_jump() = jump;
	return msg;
}

Message MessageMaker::MakeFireMessage(ADPPlayerController* Controller, const FVector Position ,const FRotator& Rotation)
{
	Message msg;
	if (Controller == nullptr)
	{
		return msg;
	}
	if (Controller->PlayerState == nullptr)
	{
		return msg;
	}

	msg.set_player_id(TCHAR_TO_UTF8(*Controller->PlayerState->GetPlayerName()));
	Gunfire gunfire;
	Vec3 position;
	position.set_x(Position.X);
	position.set_y(Position.Y);
	position.set_z(Position.Z);
	*gunfire.mutable_position() = position;
	Vec3 direction;
	direction.set_x(Rotation.Pitch);
	direction.set_y(Rotation.Yaw);
	direction.set_z(Rotation.Roll);
	*gunfire.mutable_direction() = direction;
	*msg.mutable_gunfire() = gunfire;
	return msg;
}

Message MessageMaker::MakeAimMessage(ADPPlayerController* controller, bool bAim)
{
	Message msg;
	if (controller == nullptr)
	{
		return msg;
	}
	if (controller->PlayerState == nullptr)
	{
		return msg;
	}
	msg.set_player_id(TCHAR_TO_UTF8(*controller->PlayerState->GetPlayerName()));
	AimState aim_state;
	aim_state.set_aim_state(bAim ? AimState::AIM_STATE_ACTIVE : AimState::AIM_STATE_RELEASE);
	*msg.mutable_aim_state() = aim_state;
	return msg;
}

MonsterPosition MessageMaker::MakeMonsterPositionMessage(ABaseMonsterAIController* Monster_Controller)
{
	MonsterPosition msg;
	if (Monster_Controller == nullptr)
	{
		return msg;
	}
	if (Monster_Controller->PlayerState == nullptr)
	{
		return msg;
	}
	msg.set_monster_id(std::to_string(Monster_Controller->PlayerState->GetPlayerId()));
	Vec2 position;
	position.set_x(Monster_Controller->GetPawn()->GetActorLocation().X);
	position.set_y(Monster_Controller->GetPawn()->GetActorLocation().Y);
	*msg.mutable_position() = position;
	Vec2 rotation;
	rotation.set_x(Monster_Controller->GetPawn()->GetActorRotation().Pitch);
	rotation.set_y(Monster_Controller->GetPawn()->GetActorRotation().Yaw);
	*msg.mutable_rotation() = rotation;
	Vec2 velocity;
	FVector velocity_ = Monster_Controller->GetPawn()->GetMovementComponent()->Velocity;
	velocity.set_x(velocity_.X);
	velocity.set_y(velocity_.Y);
	*msg.mutable_velocity() = velocity;
	return msg;
}

Message MessageMaker::MakeCatchMessage(ADPPlayerController* AdpPlayerController, const FHitResult& HitResult)
{
	Message msg;
	if (AdpPlayerController == nullptr)
	{
		return msg;
	}
	if (AdpPlayerController->PlayerState == nullptr)
	{
		return msg;
	}
	// 내가 잡으려고 하는 대상이 순간적으로 서버에서는 위치가 변경되었을 수도 있다.
	// 그러므로 서버에서는 클라이언트가 잡으려고 하는 대상이 누구인지 알 수 없다.
	// 그러므로 클라이언트가 잡으려고 하는 대상이 누구인지 서버에게 알려줘야 한다.
	msg.set_player_id(TCHAR_TO_UTF8(*AdpPlayerController->PlayerState->GetPlayerName()));
	
	return msg;
}
