#include "FDataHub.h"

TMap<FString, PlayerPosition> FDataHub::PlayerPositions;
TMap<FString, Movement> FDataHub::EchoData;

//XXX: Player, Enermy 등등 각자의 Map을 가질 예정.

void FDataHub::PushPlayerDA(const PlayerPosition &NewData)
{
    const FString key = NewData.player_id().c_str();
    if (!PlayerPositions.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    PlayerPositions.Add(key, NewData);
}

void FDataHub::PushEchoDA(const Movement &NewData)
{
    const FString key = NewData.player_id().c_str();
    if (!EchoData.Contains(key)) {
        // 새로운 데이터 생성 알림
    }
    EchoData.Add(key, NewData);
}

void FDataHub::RemovePlayerDA(const FString &playerId)
{
    PlayerPositions.Remove(playerId);
}
