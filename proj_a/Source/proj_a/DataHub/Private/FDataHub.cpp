#include "FDataHub.h"

#include "FNetLogger.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

TMap<ID_TYPE, ActorPosition> FDataHub::actorPosition;
TMap<ID_TYPE, Movement> FDataHub::EchoData;
TMap<ID_TYPE, Jump> FDataHub::jumpData;
TMap<ID_TYPE, Gunfire> FDataHub::gunfireData;
TMap<ID_TYPE, AimState> FDataHub::aimStateData;
TMap<ID_TYPE, MonsterPosition> FDataHub::monsterData;
TMap<ID_TYPE, Catch> FDataHub::catchData;
TMap<ID_TYPE, bool> FDataHub::returnAnimalData;
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
    if (!actorPosition.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    actorPosition.Add(key, NewData.actor_position());
}

void FDataHub::PushEchoDA(const Message& NewData)
{
    FScopeLock lock(&echoDataMutex);
    const ID_TYPE key = NewData.player_id().c_str();
    if (!EchoData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    EchoData.Add(key, NewData.movement());
}

void FDataHub::PushJumpDA(const Message& NewData)
{
    FScopeLock lock(&jumpDataMutex);
    const ID_TYPE key(NewData.player_id().c_str());
    if (!jumpData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    jumpData.Add(key, NewData.jump());
}

void FDataHub::PushGunfireDA(const Message& Msg)
{
    FScopeLock lock(&gunfireDataMutex);
    const ID_TYPE key(Msg.player_id().c_str());
    if (!gunfireData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    gunfireData.Add(key, Msg.gunfire()); 
}

void FDataHub::PushAimStateDA(const Message& Msg)
{
    FScopeLock lock(&aimStateDataMutex);
    const ID_TYPE key(Msg.player_id().c_str());
    if (!aimStateData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    aimStateData.Add(key, Msg.aim_state()); 
}

void FDataHub::PushMonsterDA(const Message& Msg)
{
    FScopeLock lock(&monsterDataMutex);
    MonsterPositionList monster_positions = Msg.monster_position();
    for (int i = 0; i < monster_positions.monster_positions_size(); i++)
    {
        const MonsterPosition& monster_position = monster_positions.monster_positions(i);
        const ID_TYPE key = monster_position.monster_id().c_str();
        if (!monsterData.Contains(key)) {
            // 새로운 데이터 생성 알림
        }
        monsterData.Add(key, monster_position);
    }
}

void FDataHub::PushCatachDA(const Message& Msg)
{
    FScopeLock lock(&catchDataMutex);
    const ID_TYPE key(Msg.player_id().c_str());
    if (!catchData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    catchData.Add(key, Msg.catch_()); 
}

void FDataHub::PushReturnAnimalDA(const FString& Key, bool bCond)
{
    FScopeLock lock(&returnAnimalDataMutex);
    if (!returnAnimalData.Contains(Key)) {
        // 새로운 데이터 생성 알림
    }
    returnAnimalData.Add(Key, bCond);
}

TOptional<ActorPosition> FDataHub::GetActorPositionData(const FString& Key)
{
    FScopeLock lock(&actorPositionMutex);
    if (actorPosition.Contains(Key)) {
        ActorPosition RetrievedPosition = actorPosition[Key];
        actorPosition.Remove(Key);
        return RetrievedPosition;
    }
    return TOptional<ActorPosition>();
}

TOptional<Movement> FDataHub::GetEchoData(const FString& Key)
{
    FScopeLock lock(&echoDataMutex);
    if (EchoData.Contains(Key)) {
        Movement RetrievedMovement = EchoData[Key];
        EchoData.Remove(Key);
        return RetrievedMovement;
    }
    return TOptional<Movement>();
}

TOptional<Jump> FDataHub::GetJumpData(const FString& Key)
{
    FScopeLock lock(&jumpDataMutex);
    if (jumpData.Contains(Key)) {
        Jump RetrievedJump = jumpData[Key];
        jumpData.Remove(Key);
        return RetrievedJump;
    }
    return TOptional<Jump>();
}

TOptional<Gunfire> FDataHub::GetGunfireData(const FString& Key)
{
    FScopeLock lock(&gunfireDataMutex);
    if (gunfireData.Contains(Key)) {
        Gunfire RetrievedGunfire = gunfireData[Key];
        gunfireData.Remove(Key);
        return RetrievedGunfire;
    }
    return TOptional<Gunfire>();
}

TOptional<AimState> FDataHub::GetAimStateData(const FString& Key)
{
    FScopeLock lock(&aimStateDataMutex);
    if (aimStateData.Contains(Key)) {
        AimState RetrievedAimState = aimStateData[Key];
        aimStateData.Remove(Key);
        return RetrievedAimState;
    }
    return TOptional<AimState>();
}

TOptional<MonsterPosition> FDataHub::GetMonsterData(const FString& Key)
{
    FScopeLock lock(&monsterDataMutex);
    if (monsterData.Contains(Key)) {
        MonsterPosition RetrievedMonsterPosition = monsterData[Key];
        monsterData.Remove(Key);
        return RetrievedMonsterPosition;
    }
    return TOptional<MonsterPosition>();
}

TOptional<Catch> FDataHub::GetCatchData(const FString& Key)
{
    FScopeLock lock(&catchDataMutex);
    if (catchData.Contains(Key)) {
        Catch RetrievedCatch = catchData[Key];
        catchData.Remove(Key);
        return RetrievedCatch;
    }
    return TOptional<Catch>();
}

TOptional<bool> FDataHub::GetReturnAnimalData(const FString& Key)
{
    FScopeLock lock(&returnAnimalDataMutex);
    if (returnAnimalData.Contains(Key)) {
        bool RetrievedReturnAnimal = returnAnimalData[Key];
        returnAnimalData.Remove(Key);
        return RetrievedReturnAnimal;
    }
    return TOptional<bool>();
}

