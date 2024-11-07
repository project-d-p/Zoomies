#pragma once

#include "CoreMinimal.h"
#include "CompileMode.h"
#include "IChatGameMode.h"
#include "JudgeData.h"
#include "ServerTimerManager.h"
#include "ServerChatManager.h"
#include "ScoreTypes.h"
#include "GameFramework/GameModeBase.h"
#include "JudgeGameMode.generated.h"

USTRUCT()
struct FPlayerInitData
{
    GENERATED_BODY()

    UPROPERTY()
    FString PlayerName;
    UPROPERTY()
    int32 Score;
    UPROPERTY()
    FString Occupation;

    FPlayerInitData() : Score(0), Occupation(TEXT("None")) {}

    FPlayerInitData(const FString& InName, int32 InScore, const FString& InOccupation = TEXT("None"))
        : PlayerName(InName), Score(InScore), Occupation(InOccupation) {}
};

USTRUCT()
struct FUIInitData
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FPlayerInitData> PlayerData;
    UPROPERTY()
    FString VoterName;
    UPROPERTY()
    bool bInitSuccessful = false;
};

UCLASS()
class PROJ_A_API AJudgeGameMode : public AGameModeBase, public IChatGameMode
{
    GENERATED_BODY()

public:
    AJudgeGameMode();

    void AddVote(EPlayerJob Occupation) { PlayerVotes.Add(Occupation); }
    virtual UServerChatManager* GetChatManager() const override { return ChatManager; }
    FUIInitData GetUiData();

private:
    EPlayerJob CollectVotingResults();
    void ProcessVotingResults();
    void EndTimer();

    int CurrentPlayerIndex = 0;
    float WAIT_TIME = Zoomies::JUDGE_TIME;
    TArray<EPlayerJob> PlayerVotes;
    int TOTAL_PLAYER;

    UPROPERTY()
    UJudgeData* JudgedInformation;

protected:
    void InitializeGameMode();
    virtual void StartPlay() override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    
    UPROPERTY()
    UServerTimerManager* TimerManager;
    UPROPERTY()
    UServerChatManager* ChatManager;
};