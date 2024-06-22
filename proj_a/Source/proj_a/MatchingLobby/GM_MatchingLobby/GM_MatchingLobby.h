// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "proj_a/Actor/Lobby/LobbyPlatform.h"
#include "../TYEP_MatchingLobby.h"
#include "GM_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API AGM_MatchingLobby : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGM_MatchingLobby();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void BeginPlay() override;
	void CheckReadyToStart();
	void FindAndStoreLobbyPlatforms();
	void CheckAndUpdateLobbyPlatform();
	void UpdatePlayerOnPlatform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerController*> PCs;
	TArray<ALobbyPlatform*> LobbyPlatforms;
	FTimerHandle UnusedHandle;
private:
	void StartGame_t() const;
protected:
	bool bIsLobbyPlatformReady = false;
};
