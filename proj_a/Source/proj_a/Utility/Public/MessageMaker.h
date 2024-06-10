#pragma once

#include "message.pb.h"
#include "DPPlayerController.h"

class MessageMaker
{
public:
	static Message MakeMessage(const ADPPlayerController* Controller, const FVector2D& Input, const FVector& Forward, const FVector& Right);
};
