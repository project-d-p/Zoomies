#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Message.pb.h"

/** Use the controller's unique ID */
#define ID_TYPE FString

class FDataHub
{
public:
	// Player Data Map
	static TMap<ID_TYPE, ActorPosition> actorPosition;
	// Test Data Map
	static TMap<ID_TYPE, Movement> EchoData;

	static TMap<ID_TYPE, Jump> jumpData;
	
	static void PushActorDA(const Message &NewData);
	static void PushEchoDA(const Message &NewData);
	static void PushJumpDA(const Message &NewData);
	static void RemovePlayerDA(const ID_TYPE &playerId);
};
