#pragma once

#include <map>

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Message.pb.h"

/** Use the controller's unique ID */
#define ID_TYPE FString

class FDataHub
{
public:
	// Player Data Map
	static std::map<ID_TYPE, ActorPosition> actorPosition;
	static std::map<ID_TYPE, Movement> EchoData;
	static std::map<ID_TYPE, Jump> jumpData;
	static std::map<ID_TYPE, Gunfire> gunfireData;
	static std::map<ID_TYPE, AimState> aimStateData;
	static std::map<ID_TYPE, MonsterPosition> monsterData;
	static std::map<ID_TYPE, Catch> catchData;
	static std::map<ID_TYPE, bool> returnAnimalData;

	static FCriticalSection actorPositionMutex;
	static FCriticalSection echoDataMutex;
	static FCriticalSection jumpDataMutex;
	static FCriticalSection gunfireDataMutex;
	static FCriticalSection aimStateDataMutex;
	static FCriticalSection monsterDataMutex;
	static FCriticalSection catchDataMutex;
	static FCriticalSection returnAnimalDataMutex;
	
	static void PushActorDA(const Message &NewData);
	static void PushEchoDA(const Message &NewData);
	static void PushJumpDA(const Message &NewData);
	static void PushGunfireDA(const Message& Msg);
	static void PushAimStateDA(const Message& Msg);
	static void PushMonsterDA(const Message& Msg);
	static void PushCatachDA(const Message& Msg);
	static void PushReturnAnimalDA(const FString& Key, bool bCond);

	static TOptional<ActorPosition> GetActorPositionData(const ID_TYPE& Key);
	static TOptional<Movement> GetEchoData(const ID_TYPE& Key);
	static TOptional<Jump> GetJumpData(const ID_TYPE& Key);
	static TOptional<Gunfire> GetGunfireData(const ID_TYPE& Key);
	static TOptional<AimState> GetAimStateData(const ID_TYPE& Key);
	static TOptional<MonsterPosition> GetMonsterData(const ID_TYPE& Key);
	static TOptional<Catch> GetCatchData(const ID_TYPE& Key);
	static TOptional<bool> GetReturnAnimalData(const ID_TYPE& Key);
};
