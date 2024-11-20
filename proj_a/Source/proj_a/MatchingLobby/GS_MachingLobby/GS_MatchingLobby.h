#pragma once

#include "CoreMinimal.h"
#include "DataManager.h"
#include "GameFramework/GameStateBase.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"
#include "GS_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API AGS_MatchingLobby : public AGameStateBase
{
	GENERATED_BODY()
public:
	AGS_MatchingLobby();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LobbyInfo)
	TArray<FLobbyInfo> LobbyInfos;
	int32 HostPlayerIndex;

	// Find the fastest player
	UPROPERTY(Replicated)
	float LowestAveragePing;
	UPROPERTY(Replicated)
	APlayerState* BestHostPlayer;

	void FindFastestPlayer();
	// void SetHostPlayer(const APlayerState* NewHostPlayer);
	void UpdateLobbyInfo() const;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowLoadingWidget();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowCountDown();

	UFUNCTION(Server, Reliable)
	void ReportPing(APlayerState* ReportingPlayer, float AveragePing);

	UFUNCTION()
	void OnRep_LobbyInfo();
	void SetPlayerReady(int32 PlayerIndex, bool bIsReady);

	UFUNCTION(BlueprintCallable, Category = "MatchLobby")
	bool AreAllPlayersReady();
	
	int32 FindIndexByPlayerId(const int32 &PlayerId) const;


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	void OnHostMigration(UWorld* World, UDataManager* DataManager);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	FDelegateHandle OnHostMigrationDelegate;
private:
	void RemovePlayerInputComponent();
};
