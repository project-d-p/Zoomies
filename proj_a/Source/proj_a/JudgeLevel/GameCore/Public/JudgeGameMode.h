#pragma once

#include "CoreMinimal.h"
#include "IChatGameMode.h"
#include "ServerTimerManager.h"
#include "ServerChatManager.h"
#include "ScoreTypes.h"
#include "GameFramework/GameModeBase.h"
#include "JudgeGameMode.generated.h"


// 추후 변경
inline FString OccupationToString(EPlayerJob Occupation)
{
    switch(Occupation)
    {
    case EPlayerJob::JOB_ARCHAEOLOGIST:
        return FString("ARCHAEOLOGIST");
    case EPlayerJob::JOB_POACHER:
        return FString("POACHER");
    case EPlayerJob::JOB_ENVIRONMENTALIST:
        return FString("ENVIRONMENTALIST");
    case EPlayerJob::JOB_RINGMASTER:
        return FString("RINGMASTER");
    case EPlayerJob::JOB_TERRORIST:
        return FString("TERRORIST");
    case EPlayerJob::JOB_CHECK:
        return FString("CHECK");
    case EPlayerJob::JOB_CROSS:
        return FString("CROSS");
    default:
        return FString("UNKNOWN");
    }
}

UCLASS()
class PROJ_A_API AJudgeGameMode : public AGameModeBase, public IChatGameMode
{
    GENERATED_BODY()

public:
    AJudgeGameMode();

    void AddVote(EPlayerJob Occupation) { PlayerVotes.Add(Occupation); }
    virtual UServerChatManager* GetChatManager() const override { return ChatManager; }

private:
    EPlayerJob CollateVotingResults();
    void ProcessVotingResults();
    void EndTimer();

    int CurrentPlayerIndex = 0;
    const float WAIT_TIME = 10.f;
    TArray<EPlayerJob> PlayerVotes;
    
protected:
    virtual void StartPlay() override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    
    UPROPERTY()
    UServerTimerManager* TimerManager;
    UPROPERTY()
    UServerChatManager* ChatManager;
};