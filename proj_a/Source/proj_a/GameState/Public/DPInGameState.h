#pragma once

#include "GameFramework/GameStateBase.h"
#include "ClientTimerManager.h"
#include "ClientScoreMananger.h"
#include "ChatManager.h"
#include "IChatGameState.h"
#include "NetworkFailureManager.h"
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

	void AddConnectedPlayer() { ConnectedPlayers++; }
	bool AreAllPlayersConnected() { return ConnectedPlayers == ExpectedPlayers; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerJob() const;
	
	/*
	 * TEST: COMMENT
	UPROPERTY(ReplicatedUsing=OnRep_ServerTraveled)
	bool bServerTraveled;
	*/
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnHostMigration(UWorld* World, UDataManager* DataManager);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	int32 ConnectedPlayers = 0;
	const int32 ExpectedPlayers = 4;
	
	/*
	 * TEST: COMMENT
	UFUNCTION()
	void OnRep_ServerTraveled() const;
	 */

	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;
	UPROPERTY()
	UChatManager* ChatManager;
	FDelegateHandle OnHostMigrationDelegate;
};

