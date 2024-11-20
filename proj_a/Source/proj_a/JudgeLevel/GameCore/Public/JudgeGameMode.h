#pragma once

#include "CoreMinimal.h"
#include "CompileMode.h"
#include "IChatGameMode.h"
#include "JudgeData.h"
#include "JudgePlayerState.h"
#include "ServerTimerManager.h"
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
    UPROPERTY()
    int32 PlayerId;
    UPROPERTY()
    int32 CameraIndex;

    FPlayerInitData() : Score(0), Occupation(TEXT("None")), PlayerId(0), CameraIndex(0) {}

    FPlayerInitData(const FString& InName, int32 InScore, const FString& InOccupation = TEXT("None"))
        : PlayerName(InName), Score(InScore), Occupation(InOccupation) , PlayerId(-1), CameraIndex(-1) {}
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
    virtual void PostLogin(APlayerController* NewPlayer) override;

    void AddVote(const FString& VotedName, EPlayerJob Occupation);
    virtual UServerChatManager* GetChatManager() const override { return ChatManager; }
    FUIInitData GetUiData();

private:
    void CollectVotingResults(const FString& CurrentVotedPlayer);
    AJudgePlayerState* GetPlayerStateFromPlayerName(const FString& PlayerName);
    AJudgePlayerState* GetCurrentVotedPlayerState();
    void HandlePlayerStateNull();
    void ProcessVotingResults();
    bool IsJudgedAllPlayers();
    void EndTimer();

    int CurrentPlayerIndex = 0;
    float WAIT_TIME = Zoomies::JUDGE_TIME;
    TMap<FString, TArray<EPlayerJob>> PlayerVotes;
    int TOTAL_PLAYER;

    UPROPERTY()
    UJudgeData* JudgedInformation;
    bool bVotingEnd;

protected:
    void InitializeGameMode();
    virtual void StartPlay() override;
    virtual void Logout(AController* Exiting) override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;
    
    UPROPERTY()
    UServerTimerManager* TimerManager;
    UPROPERTY()
    UServerChatManager* ChatManager;
};