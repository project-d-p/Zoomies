#include "MessageMaker.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "PlayerName.h"
#include "GameFramework/PlayerState.h"

Message MessageMaker::MakeMovementMessage(const ADPPlayerController* Controller, const FVector2d& ActionValue, const FRotator& Rotation, const FVector& Velocity)
{
	Message msg;
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
	// Vec3 rotation;
	float delta = Controller->GetWorld()->GetDeltaSeconds();
	movement.set_delta_time(delta);
	*msg.mutable_movement() = movement;
	// msg.set_timestamp();
	return msg;
}

Message MessageMaker::MakePositionMessage(const ADPPlayerController* Controller)
{
	Message msg;
	msg.set_player_id(TCHAR_TO_UTF8(*Controller->PlayerState->GetPlayerName()));
	ActorPosition actor_position;
	Vec3 position;
	position.set_x(Controller->GetPawn()->GetActorLocation().X);
	position.set_y(Controller->GetPawn()->GetActorLocation().Y);
	position.set_z(Controller->GetPawn()->GetActorLocation().Z);
	*actor_position.mutable_position() = position;
	// Vec3 rotation;
	// rotation.set_x(Controller->GetPawn()->GetActorRotation().Roll);
	// rotation.set_y(Controller->GetPawn()->GetActorRotation().Pitch);
	// rotation.set_z(Controller->GetPawn()->GetActorRotation().Yaw);
	// *actor_position.mutable_rotation() = rotation;
	Vec3 velocity;
	velocity.set_x(Controller->GetPawn()->GetVelocity().X);
	velocity.set_y(Controller->GetPawn()->GetVelocity().Y);
	velocity.set_z(Controller->GetPawn()->GetVelocity().Z);
	*actor_position.mutable_velocity() = velocity;
	*msg.mutable_actor_position() = actor_position;
	return msg;
}
