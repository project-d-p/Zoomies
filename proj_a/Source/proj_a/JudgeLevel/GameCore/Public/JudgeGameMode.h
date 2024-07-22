#pragma once

#include "CoreMinimal.h"
#include "IChatGameMode.h"
#include "JudgePlayerController.h"
#include "ServerTimerManager.h"
#include "ServerChatManager.h"
#include "GameFramework/GameModeBase.h"
#include "JudgeGameMode.generated.h"

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

    int CurrentPlayerIndex = 0;
    TArray<EOccupation> PlayerVotes;
    
protected:
    virtual void StartPlay() override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    
    UPROPERTY()
    UServerTimerManager* TimerManager;
    UPROPERTY()
    UServerChatManager* ChatManager;
};