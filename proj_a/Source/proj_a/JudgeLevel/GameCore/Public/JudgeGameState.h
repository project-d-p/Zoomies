#pragma once

#include "ClientTimerManager.h"
#include "IChatGameState.h"
#include "ChatManager.h"
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
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;
	UPROPERTY()
	UChatManager* ChatManager;
    USoundBase *TurnStartSound;
};