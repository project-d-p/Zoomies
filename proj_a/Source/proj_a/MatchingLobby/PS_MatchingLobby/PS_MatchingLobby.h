// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "PS_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API APS_MatchingLobby : public APlayerState
{
	GENERATED_BODY()
public:
	/* Register & Unregister a player with the online subsystem */
	virtual void RegisterPlayerWithSession(bool bWasFromInvite) override;
	virtual void UnregisterPlayerWithSession() override;

	/*
	 * Call RegisterPlayerWithSession for all players (escpecially for host) during seamless travel
	 * Becuase OnRep_UniqueID() is not calling for host during Seamless Travel
	 */ 
	virtual void CopyProperties(APlayerState* PlayerState) override;
protected:
	void SetSessionName();
	bool SeamlessTravling;
};