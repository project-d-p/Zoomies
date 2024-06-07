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
	
	UFUNCTION()
	void OnRep_ReadyPlayers();
	
	void SetPlayerReady(int32 PlayerIndex, bool bIsReady);

	UFUNCTION(BlueprintCallable, Category = "GameState")
	bool AreAllPlayersReady() const;
	
	APlayerState* FindFastestPlayer() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	
};
