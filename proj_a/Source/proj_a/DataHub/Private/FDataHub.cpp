#include "FDataHub.h"

#include "FNetLogger.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

std::map<ID_TYPE, ActorPosition> FDataHub::actorPosition;
std::map<ID_TYPE, Movement> FDataHub::EchoData;
std::map<ID_TYPE, Jump> FDataHub::jumpData;
std::map<ID_TYPE, Gunfire> FDataHub::gunfireData;
std::map<ID_TYPE, AimState> FDataHub::aimStateData;
std::map<ID_TYPE, MonsterPosition> FDataHub::monsterData;
std::map<ID_TYPE, Catch> FDataHub::catchData;
std::map<ID_TYPE, bool> FDataHub::returnAnimalData;
//XXX: Player, Enermy 등등 각자의 Map을 가질 예정.

FCriticalSection FDataHub::actorPositionMutex;
FCriticalSection FDataHub::echoDataMutex;
FCriticalSection FDataHub::jumpDataMutex;
FCriticalSection FDataHub::gunfireDataMutex;
FCriticalSection FDataHub::aimStateDataMutex;
FCriticalSection FDataHub::monsterDataMutex;
FCriticalSection FDataHub::catchDataMutex;
FCriticalSection FDataHub::returnAnimalDataMutex;

void FDataHub::PushActorDA(const Message& NewData)
{
    FScopeLock lock(&actorPositionMutex);
    const ID_TYPE key(NewData.player_id().c_str());
    if (!actorPosition.contains(key)) {
        // 새로운 데이터 생성 알림
    }
    actorPosition[key] = NewData.actor_position();
}

void FDataHub::PushEchoDA(const Message& NewData)
{
    FScopeLock lock(&echoDataMutex);
    const ID_TYPE key = NewData.player_id().c_str();
    if (!EchoData.contains(key)) {
        // 새로운 데이터 생성 알림
    }
    EchoData[key] = NewData.movement();
}

void FDataHub::PushJumpDA(const Message& NewData)
{
    FScopeLock lock(&jumpDataMutex);
    const ID_TYPE key(NewData.player_id().c_str());
    if (!jumpData.contains(key)) {
        // 새로운 데이터 생성 알림
    }
    jumpData[key] = NewData.jump();
}

void FDataHub::PushGunfireDA(const Message& Msg)
{
    FScopeLock lock(&gunfireDataMutex);
    const ID_TYPE key(Msg.player_id().c_str());
    if (!gunfireData.contains(key)) {
        // 새로운 데이터 생성 알림
    }
    gunfireData[key] = Msg.gunfire();
}

void FDataHub::PushAimStateDA(const Message& Msg)
{
    FScopeLock lock(&aimStateDataMutex);
    const ID_TYPE key(Msg.player_id().c_str());
    if (!aimStateData.contains(key)) {
        // 새로운 데이터 생성 알림
    }
    aimStateData[key] = Msg.aim_state();
}

void FDataHub::PushMonsterDA(const Message& Msg)
{
    FScopeLock lock(&monsterDataMutex);
    MonsterPositionList monster_positions = Msg.monster_position();
    for (int i = 0; i < monster_positions.monster_positions_size(); i++)
    {
        const MonsterPosition& monster_position = monster_positions.monster_positions(i);
        const ID_TYPE key = monster_position.monster_id().c_str();
        if (!monsterData.contains(key)) {
            // 새로운 데이터 생성 알림
        }
        monsterData[key] = monster_position;
    }
}

void FDataHub::PushCatachDA(const Message& Msg)
{
    FScopeLock lock(&catchDataMutex);
    const ID_TYPE key(Msg.player_id().c_str());
    if (!catchData.contains(key)) {
        // 새로운 데이터 생성 알림
    }
    catchData[key] = Msg.catch_();
}

void FDataHub::PushReturnAnimalDA(const FString& Key, bool bCond)
{
    FScopeLock lock(&returnAnimalDataMutex);
    if (!returnAnimalData.contains(Key)) {
        // 새로운 데이터 생성 알림
    }
    returnAnimalData[Key] = bCond;
}

TOptional<ActorPosition> FDataHub::GetActorPositionData(const FString& Key)
{
    FScopeLock lock(&actorPositionMutex);
    if (actorPosition.contains(Key)) {
        ActorPosition RetrievedPosition = actorPosition[Key];
        actorPosition.erase(Key);
        return RetrievedPosition;
    }
    return TOptional<ActorPosition>();
}

TOptional<Movement> FDataHub::GetEchoData(const FString& Key)
{
    FScopeLock lock(&echoDataMutex);
    if (EchoData.contains(Key)) {
        Movement RetrievedMovement = EchoData[Key];
        EchoData.erase(Key);
        return RetrievedMovement;
    }
    return TOptional<Movement>();
}

TOptional<Jump> FDataHub::GetJumpData(const FString& Key)
{
    FScopeLock lock(&jumpDataMutex);
    if (jumpData.contains(Key)) {
        Jump RetrievedJump = jumpData[Key];
        jumpData.erase(Key);
        return RetrievedJump;
    }
    return TOptional<Jump>();
}

TOptional<Gunfire> FDataHub::GetGunfireData(const FString& Key)
{
    FScopeLock lock(&gunfireDataMutex);
    if (gunfireData.contains(Key)) {
        Gunfire RetrievedGunfire = gunfireData[Key];
        gunfireData.erase(Key);
        return RetrievedGunfire;
    }
    return TOptional<Gunfire>();
}

TOptional<AimState> FDataHub::GetAimStateData(const FString& Key)
{
    FScopeLock lock(&aimStateDataMutex);
    if (aimStateData.contains(Key)) {
        AimState RetrievedAimState = aimStateData[Key];
        aimStateData.erase(Key);
        return RetrievedAimState;
    }
    return TOptional<AimState>();
}

TOptional<MonsterPosition> FDataHub::GetMonsterData(const FString& Key)
{
    FScopeLock lock(&monsterDataMutex);
    if (monsterData.contains(Key)) {
        MonsterPosition RetrievedMonsterPosition = monsterData[Key];
        monsterData.erase(Key);
        return RetrievedMonsterPosition;
    }
    return TOptional<MonsterPosition>();
}

TOptional<Catch> FDataHub::GetCatchData(const FString& Key)
{
    FScopeLock lock(&catchDataMutex);
    if (catchData.contains(Key)) {
        Catch RetrievedCatch = catchData[Key];
        catchData.erase(Key);
        return RetrievedCatch;
    }
    return TOptional<Catch>();
}

TOptional<bool> FDataHub::GetReturnAnimalData(const FString& Key)
{
    FScopeLock lock(&returnAnimalDataMutex);
    if (returnAnimalData.contains(Key)) {
        bool RetrievedReturnAnimal = returnAnimalData[Key];
        returnAnimalData.erase(Key);
        return RetrievedReturnAnimal;
    }
    return TOptional<bool>();
}

