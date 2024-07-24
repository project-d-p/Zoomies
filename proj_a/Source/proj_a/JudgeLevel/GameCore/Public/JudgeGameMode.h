#pragma once

#include "CoreMinimal.h"
#include "IChatGameMode.h"
#include "ServerTimerManager.h"
#include "ServerChatManager.h"
#include "EnumTypes.h"
#include "GameFramework/GameModeBase.h"
#include "JudgeGameMode.generated.h"


// 추후 변경
inline FString OccupationToString(EOccupation Occupation)
{
    switch(Occupation)
    {
    case EOccupation::ARCHAEOLOGIST:
        return FString("ARCHAEOLOGIST");
    case EOccupation::POACHER:
        return FString("POACHER");
    case EOccupation::ENVIRONMENTALIST:
        return FString("ENVIRONMENTALIST");
    case EOccupation::RINGMASTER:
        return FString("RINGMASTER");
    case EOccupation::TERRORIST:
        return FString("TERRORIST");
    case EOccupation::CHECK:
        return FString("CHECK");
    case EOccupation::CROSS:
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

    void SetVote(EOccupation Occupation) { PlayerVotes.Add(Occupation); }
    virtual UServerChatManager* GetChatManager() const override { return ChatManager; }

private:
    EOccupation CollateVotingResults();
    void ProcessVotingResults();
    void EndTimer();

    int CurrentPlayerIndex = -1;
    const float WAIT_TIME = 10.f;
    TArray<EOccupation> PlayerVotes;
    
protected:
    virtual void StartPlay() override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    
    UPROPERTY()
    UServerTimerManager* TimerManager;
    UPROPERTY()
    UServerChatManager* ChatManager;
};