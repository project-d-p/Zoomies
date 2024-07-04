// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamInvite.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

void USteamInvite::ShowSteamOverlayInviteDialog()
{
	if (SteamFriends())
	{
		SteamFriends()->ActivateGameOverlayInviteDialog(SteamUser()->GetSteamID());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SteamFriends interface not available"));
	}
}

void USteamInvite::InitializeInviteCallback()
{
	ConfigureSteamSessionInvite();

	if (SteamFriends())
	{
		SteamFriends()->SetRichPresence("status", "In Game");
		SteamFriends()->SetRichPresence("connect", TCHAR_TO_UTF8(*InviteURL));
	}
}

void USteamInvite::ConfigureSteamSessionInvite()
{
	CurrentSessionInfo = SetSessionInfoByCurrentSession();

	if (CurrentSessionInfo.Port == 0 || CurrentSessionInfo.IPAddress == TEXT("") || CurrentSessionInfo.SessionID == TEXT(""))
	{
		//logging on screen about fail to get session info
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				30.f,
				FColor::Red,
				FString::Printf(TEXT("Failed to get session info")));
		}
	}
	FString GameAppID = GAME_APPID;
	InviteURL = FString::Printf(TEXT("steam://run/%s//-connect %s:%d/%s"),
								*GameAppID,
								*CurrentSessionInfo.IPAddress,
								CurrentSessionInfo.Port,
								*CurrentSessionInfo.SessionID);
}

FCurrentSessionInfo USteamInvite::SetSessionInfoByCurrentSession()
{
	FCurrentSessionInfo SessionInfo = {
		TEXT(""),
		0,
		TEXT("")
	};
	if (IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			if (FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession))
			{
				if (Session->SessionInfo.IsValid())
				{
					FString IPAddress = Session->SessionInfo->GetSessionId().ToString();
					if (IPAddress != TEXT(""))
						SessionInfo.IPAddress = IPAddress;
					FString SessionID = Session->SessionInfo->GetSessionId().ToString();
					if (SessionID != TEXT(""))
						SessionInfo.SessionID = SessionID;
					FString HostConnectString;
					if (SessionInterface->GetResolvedConnectString(Session->SessionName, HostConnectString))
					{
						int32 ColonIndex;
						if (HostConnectString.FindChar(':', ColonIndex))
						{
							int32 Port = FCString::Atoi(*HostConnectString.Mid(ColonIndex + 1));
							if (Port > 0)
								SessionInfo.Port = Port;
						}
					}
				}
			}
		}
	}
	return SessionInfo; 
}