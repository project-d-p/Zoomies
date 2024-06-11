#include "MessageMaker.h"
#include "DPPlayerController.h"
#include "PlayerName.h"

Message MessageMaker::MakeMessage(const ADPPlayerController* Controller, const FVector2D& Input, const FVector& Forward, const FVector& Right, const FVector& Velocity)
{
	Message msg;
	// msg.set_player_id(TCHAR_TO_UTF8(*(Controller->GetPawn()->GetName())));

	// 아래의 방법으로 하게 되면 쓰레기 값이 출력됨. 메모리 반환 이슈
	// std::string player_id("DPCharacter_1");
	// msg.set_player_id(player_id);

	// 올바른 방법
	// TODO: 서버하고 클라이언트가 같은지 테스트 필요
	// msg.set_player_id(TCHAR_TO_UTF8(*(Controller->GetPawn()->GetName())));
	msg.set_player_id(PlayerName::GetNamePlayerOne());
	Movement movement;
	Vec3 progress_vector;
	progress_vector.set_x(Input.X);
	progress_vector.set_y(Input.Y);
	*movement.mutable_progess_vector() = progress_vector;
	Vec3 forward;
	forward.set_x(Forward.X);
	forward.set_y(Forward.Y);
	forward.set_z(Forward.Z);
	*movement.mutable_forward_vector() = forward;
	Vec3 right;
	right.set_x(Right.X);
	right.set_y(Right.Y);
	right.set_z(Right.Z);
	*movement.mutable_right_vector() = right;
	Vec3 velocity;
	velocity.set_x(Velocity.X);
	velocity.set_y(Velocity.Y);
	velocity.set_z(Velocity.Z);
	*movement.mutable_velocity() = velocity;
	movement.set_velocity_size(Velocity.Size());
	// Vec3 rotation;
	*msg.mutable_movement() = movement;
	// msg.set_timestamp();
	return msg;
}

Message MessageMaker::MakeMessage(const ADPPlayerController* Controller)
{
	Message msg;
	msg.set_player_id(TCHAR_TO_UTF8(*(Controller->GetPackage()->GetName())));
	ActorPosition actor_position;
	Vec3 position;
	position.set_x(Controller->GetPawn()->GetActorLocation().X);
	position.set_y(Controller->GetPawn()->GetActorLocation().Y);
	position.set_z(Controller->GetPawn()->GetActorLocation().Z);
	*actor_position.mutable_position() = position;
	Vec3 rotation;
	rotation.set_x(Controller->GetPawn()->GetActorRotation().Roll);
	rotation.set_y(Controller->GetPawn()->GetActorRotation().Pitch);
	rotation.set_z(Controller->GetPawn()->GetActorRotation().Yaw);
	*actor_position.mutable_rotation() = rotation;
	Vec3 velocity;
	velocity.set_x(Controller->GetPawn()->GetVelocity().X);
	velocity.set_y(Controller->GetPawn()->GetVelocity().Y);
	velocity.set_z(Controller->GetPawn()->GetVelocity().Z);
	*actor_position.mutable_velocity() = velocity;
	*msg.mutable_actor_position() = actor_position;
	return msg;
}
