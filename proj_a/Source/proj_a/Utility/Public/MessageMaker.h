#pragma once

#include "message.pb.h"
#include "DPPlayerController.h"

class MessageMaker
{
public:
	static Message MakeMovementMessage(const ADPPlayerController* Controller, const FVector2d& ActionValue, const FRotator& Rotation, const FVector& Velocity);
	static Message MakePositionMessage(const ADPPlayerController* Controller);
	static Message MakeJumpMessage(ADPPlayerController* AdpPlayerController);
	static Message MakeFireMessage(ADPPlayerController* Controller, const FRotator& Rotation, const FString& Target = "none");
	static Message MakeAimMessage(ADPPlayerController* controller, bool bAim);
};
