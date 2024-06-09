#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API AGS_MatchingLobby : public AGameStateBase
{
	GENERATED_BODY()
public:
	AGS_MatchingLobby();

	// Replicated variable & function
	UPROPERTY(ReplicatedUsing = OnRep_ReadyPlayers)
	TArray<bool> ReadyPlayers;
	int32 HostPlayerIndex;

	// Find the fastest player
	UPROPERTY(Replicated)
	float LowestAveragePing;
	UPROPERTY(Replicated)
	APlayerState* BestHostPlayer;
	
	void FindFastestPlayer();
	void SetHostPlayer(const APlayerState* NewHostPlayer);

	UFUNCTION(Server, Reliable)
	void ReportPing(APlayerState* ReportingPlayer, float AveragePing);


	// Set the player ready
	UFUNCTION()
	void OnRep_ReadyPlayers();
	void SetPlayerReady(int32 PlayerIndex, bool bIsReady);

	UFUNCTION(BlueprintCallable, Category = "GameState")
	bool AreAllPlayersReady();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};