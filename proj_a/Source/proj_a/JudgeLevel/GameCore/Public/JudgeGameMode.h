#pragma once

#include "CoreMinimal.h"
#include "ServerTimerManager.h"
#include "GameFramework/GameMode.h"
#include "JudgeGameMode.generated.h"

UCLASS()
class PROJ_A_API AJudgeGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AJudgeGameMode();

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;
    virtual void StartPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    
    void SetExpectedPlayerCount(int32 Count);
    bool AreAllPlayersConnected() const;

    UServerTimerManager* GetTimerManager() const { return TimerManager; }
protected:
    int32 ExpectedPlayerCount;
    int32 ConnectedPlayerCount;

    UPROPERTY(Replicated)
    bool bGameInProgress;
    UPROPERTY()
    UServerTimerManager* TimerManager;
    
    void CheckAllPlayersConnected();
    void OnAllPlayersConnected();
};