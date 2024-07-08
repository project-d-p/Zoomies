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
	static TMap<ID_TYPE, Gunfire> gunfireData;
	static TMap<ID_TYPE, AimState> aimStateData;
	static TMap<ID_TYPE, MonsterPosition> monsterData;
	static TMap<ID_TYPE, Catch> catchData;
	static TMap<ID_TYPE, bool> returnAnimalData;
	
	static void PushActorDA(const Message &NewData);
	static void PushEchoDA(const Message &NewData);
	static void PushJumpDA(const Message &NewData);
	static void RemovePlayerDA(const ID_TYPE &playerId);
	static void PushGunfireDA(const Message& Msg);
	static void PushAimStateDA(const Message& Msg);
	static void PushMonsterDA(const Message& Msg);
	static void PushCatachDA(const Message& Msg);
	static void PushReturnAnimalDA(const FString& Key, bool bCond);
};
