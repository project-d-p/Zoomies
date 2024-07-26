#pragma once

#include "GameFramework/GameStateBase.h"
#include "ClientTimerManager.h"
#include "ClientScoreMananger.h"
#include "ChatManager.h"
#include "IChatGameState.h"
#include "DPInGameState.generated.h"

UCLASS()
class PROJ_A_API ADPInGameState : public AGameStateBase, public IChatGameState
{
	GENERATED_BODY()
public:
	ADPInGameState();
	
	UPROPERTY(Replicated)
	UClientScoreMananger* ScoreManager;

	virtual UChatManager* GetChatManager() const override { return ChatManager; }
	UClientTimerManager* GetTimerManager() const { return TimerManager; }
	
	UPROPERTY(ReplicatedUsing=OnRep_ServerTraveled)
	bool bServerTraveled;
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ServerTraveled() const;

	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;
	UPROPERTY()
	UChatManager* ChatManager;
};
