#include "FDataHub.h"

#include "FNetLogger.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

TMap<ID_TYPE, ActorPosition> FDataHub::actorPosition;
TMap<ID_TYPE, Movement> FDataHub::EchoData;

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

void FDataHub::RemovePlayerDA(const FString& playerId)
{
    actorPosition.Remove(playerId);
}
