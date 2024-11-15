// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_MatchingLobby.h"

#include "OnlineSubsystemUtils.h"
#include "Net/OnlineEngineInterface.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

void APS_MatchingLobby::RegisterPlayerWithSession(bool bWasFromInvite)
{
	SetSessionName();
	
	// Super::RegisterPlayerWithSession(bWasFromInvite);

	if (GetNetMode() != NM_Standalone)
	{
		if (GetUniqueId().IsValid()) // May not be valid if this is was created via DebugCreatePlayer
		{
			// Register the player as part of the session
			if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), this->SessionName))
			{
				UOnlineEngineInterface::Get()->RegisterPlayer(GetWorld(), this->SessionName, GetUniqueId(), bWasFromInvite);
			}
		}
	}
}

void APS_MatchingLobby::UnregisterPlayerWithSession()
{
	SetSessionName();
	
	// Super::UnregisterPlayerWithSession();

	if ((GetNetMode() == NM_Client && GetUniqueId().IsValid()) && !this->SeamlessTravling)
	{
		if (this->SessionName != NAME_None)
		{
			if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), this->SessionName))
			{
				UOnlineEngineInterface::Get()->UnregisterPlayer(GetWorld(), this->SessionName, GetUniqueId());
			}
		}
	}
}

void APS_MatchingLobby::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	SeamlessTravling = true;
	PlayerState->OnRep_UniqueId();
}

void APS_MatchingLobby::SetSessionName()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		SessionName = GameInstance->GetSessionName();
	}
}
