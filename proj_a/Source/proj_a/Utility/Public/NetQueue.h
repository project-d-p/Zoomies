#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"

struct FInputData
{
	FVector2D InputVector;
	int32 Type;
};

extern TQueue<FInputData, EQueueMode::Spsc> InputQueue;