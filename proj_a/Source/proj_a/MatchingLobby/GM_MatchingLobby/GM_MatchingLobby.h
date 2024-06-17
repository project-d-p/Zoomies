// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
	void CheckLobbyReady();
	
	TArray<APlayerController*> PCs;
private:
	void StartGame_t() const;
protected:
	UClass* LobbyPlatformClass;
	TArray<AActor*> LobbyPlatforms;
	bool bIsLobbyPlatformReady = false;
};
