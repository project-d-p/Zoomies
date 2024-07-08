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

void FDataHub::PushActorDA(const Message& NewData)
{
    const ID_TYPE key(NewData.player_id().c_str());
    if (!actorPosition.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    actorPosition.Add(key, NewData.actor_position());
}

void FDataHub::PushEchoDA(const Message& NewData)
{
    const ID_TYPE key = NewData.player_id().c_str();
    if (!EchoData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    EchoData.Add(key, NewData.movement());
}

void FDataHub::PushJumpDA(const Message& NewData)
{
    const ID_TYPE key(NewData.player_id().c_str());
    if (!jumpData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    jumpData.Add(key, NewData.jump());
}

void FDataHub::RemovePlayerDA(const FString& playerId)
{
    actorPosition.Remove(playerId);
}

void FDataHub::PushGunfireDA(const Message& Msg)
{
    const ID_TYPE key(Msg.player_id().c_str());
    if (!gunfireData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    gunfireData.Add(key, Msg.gunfire()); 
}

void FDataHub::PushAimStateDA(const Message& Msg)
{
    const ID_TYPE key(Msg.player_id().c_str());
    if (!aimStateData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    aimStateData.Add(key, Msg.aim_state()); 
}

void FDataHub::PushMonsterDA(const Message& Msg)
{
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
    const ID_TYPE key(Msg.player_id().c_str());
    if (!catchData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    catchData.Add(key, Msg.catch_()); 
}

void FDataHub::PushReturnAnimalDA(const FString& Key, bool bCond)
{
    if (!returnAnimalData.Contains(Key)) {
        // 새로운 데이터 생성 알림
    }
    returnAnimalData.Add(Key, bCond);
}
