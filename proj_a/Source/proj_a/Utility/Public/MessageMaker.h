#pragma once

#include <vector>

#include "BaseMonsterAIController.h"
#include "message.pb.h"
#include "DPPlayerController.h"

class MessageMaker
{
public:
	static Message MakeMovementMessage(const ADPPlayerController* Controller, const FVector2d& ActionValue, const FRotator& Rotation, const FVector& Velocity);
	static Message MakePositionMessage(const ADPPlayerController* Controller);
	static Message MakeJumpMessage(ADPPlayerController* AdpPlayerController);
	static Message MakeFireMessage(ADPPlayerController* Controller, const FVector Position, const FRotator& Rotation);
	static Message MakeAimMessage(ADPPlayerController* controller, bool bAim);
	static MonsterPosition MakeMonsterPositionMessage(ABaseMonsterAIController* Monster_Controller, int index);
	static Message MakeCatchMessage(ADPPlayerController* AdpPlayerController, const FHitResult& HitResult);
};
