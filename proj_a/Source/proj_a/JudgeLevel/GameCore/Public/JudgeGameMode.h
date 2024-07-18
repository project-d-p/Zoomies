#pragma once

#include "CoreMinimal.h"
#include "ServerTimerManager.h"
#include "ServerChatManager.h"
#include "GameFramework/GameMode.h"
#include "JudgeGameMode.generated.h"

UCLASS()
class PROJ_A_API AJudgeGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AJudgeGameMode();

    UServerChatManager* GetChatManager() const { return ChatManager; }

private:
    void EndTimer();
    void SetExpectedPlayerCount(int32 Count);
    bool AreAllPlayersConnected() const;
    
protected:
    virtual void StartPlay() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;
    
    int32 ExpectedPlayerCount;
    int32 ConnectedPlayerCount;

    UPROPERTY(Replicated)
    bool bGameInProgress;
    UPROPERTY()
    UServerTimerManager* TimerManager;
    UPROPERTY()
    UServerChatManager* ChatManager;
    
    void CheckAllPlayersConnected();
    void OnAllPlayersConnected();
};