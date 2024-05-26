#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Message.pb.h"

class FDataHub
{
public:
	// Player Data Map
	static TMap<FString, ActorPosition> actorPosition;
	// Test Data Map
	static TMap<FString, Movement> EchoData;
	
	static void PushActorDA(const ActorPosition &NewData);
	static void PushEchoDA(const Movement &NewData);
	static void RemovePlayerDA(const FString &playerId);
};
