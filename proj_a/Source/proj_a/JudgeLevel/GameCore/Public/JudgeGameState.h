#pragma once

#include "ClientTimerManager.h"
#include "IChatGameState.h"
#include "ChatManager.h"
#include "DataManager.h"
#include "JudgeData.h"
#include "NetworkFailureManager.h"
#include "JudgeGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "JudgeGameState.generated.h"

UCLASS()
class PROJ_A_API AJudgeGameState : public AGameStateBase, public IChatGameState
{
	GENERATED_BODY()
public:
	AJudgeGameState();

	virtual UChatManager* GetChatManager() const override { return ChatManager; }
	UClientTimerManager* GetTimerManager() const { return TimerManager; }

	UFUNCTION(NetMulticast, Reliable)
	void NotifyTimerEnd();

	UFUNCTION(NetMulticast, Reliable)
	void SetVoterName(const FString& Name);

	UPROPERTY(Replicated)
	TArray<FPlayerInitData> GS_PlayerData;
	
	FString CurrentVotedPlayerName;
protected:
	virtual void BeginPlay() override;
	void OnHostMigration(UWorld* World, UDataManager* DataManager);

private:
	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;
	UPROPERTY()
	UChatManager* ChatManager;
    USoundBase *TurnStartSound;
	FDelegateHandle OnHostMigrationDelegate;
	UPROPERTY()
	UJudgeData* JudgedInformation;
};