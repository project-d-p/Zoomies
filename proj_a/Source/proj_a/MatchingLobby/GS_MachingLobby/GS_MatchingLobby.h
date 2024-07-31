#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/GameStateBase.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"
#include "steam_api.h"
#include "GS_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API AGS_MatchingLobby : public AGameStateBase
{
	GENERATED_BODY()
public:
	AGS_MatchingLobby();

	UPROPERTY(Replicated)
	TArray<bool> ReadyPlayers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LobbyInfo)
	TArray<FLobbyInfo> LobbyInfos;
	int32 HostPlayerIndex;

	// Find the fastest player
	UPROPERTY(Replicated)
	float LowestAveragePing;
	UPROPERTY(Replicated)
	APlayerState* BestHostPlayer;
	
	void FindFastestPlayer();
	void SetHostPlayer(const APlayerState* NewHostPlayer);
	void UpdateLobbyInfo() const;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowLoadingWidget();

	UFUNCTION(Server, Reliable)
	void ReportPing(APlayerState* ReportingPlayer, float AveragePing);

	UFUNCTION()
	void OnRep_LobbyInfo();
	void SetPlayerReady(int32 PlayerIndex, bool bIsReady);

	UFUNCTION(BlueprintCallable, Category = "MatchLobby")
	bool AreAllPlayersReady();


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};