#pragma once

#include "message.pb.h"
#include "DPPlayerController.h"

class MessageMaker
{
public:
	static Message MakeMovementMessage(const ADPPlayerController* Controller, const FVector2d& ActionValue, const FRotator& Rotation, const FVector& Velocity);
	static Message MakePositionMessage(const ADPPlayerController* Controller);
	static Message MakeJumpMessage(ADPPlayerController* AdpPlayerController);
};
