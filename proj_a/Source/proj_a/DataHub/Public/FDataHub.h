#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Message.pb.h"

class FDataHub
{
public:
	// Player Data Map
	static TMap<FString, PlayerPosition> PlayerPositions;
	// Test Data Map
	static TMap<FString, Movement> EchoData;
	
	static void PushPlayerDA(const PlayerPosition &NewData);
	static void PushEchoDA(const Movement &NewData);
	static void RemovePlayerDA(const FString &playerId);
};
