#include "GI_Zoomies.h"

#include "CreateSessionCallbackProxy.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "steam_api.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

void UGI_Zoomies::OnHostDisconnected()
{
	if (ADPPlayerController* PC = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroying PlayerController"));
		PC->ReleaseMemory();
		PC->Destroy();
	}
	UGameplayStatics::OpenLevel(GetWorld(), FName("lobbyLevel?closed"));
}

void UGI_Zoomies::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
                                       const FString& String)
{
	if (Arg == ENetworkFailure::ConnectionLost || Arg == ENetworkFailure::ConnectionTimeout)
	{
		UE_LOG(LogTemp, Warning, TEXT("Network failure In HandleNetworkFailure: %s"), *String);
		OnHostDisconnected();
	}
}

void UGI_Zoomies::Init()
{
	Super::Init();
	CheckSteamInit();
}

void UGI_Zoomies::ReturnToMainMenu()
{
	Super::ReturnToMainMenu();
}

// matching session Functions
void UGI_Zoomies::StartMatchMaking()
{
	FindSession_t();
}

IOnlineSessionPtr UGI_Zoomies::GetOnlineSessionInterface() const
{
	return session_interface_;
}

void UGI_Zoomies::FindSession_t()
{
	session_search_ = MakeShareable(new FOnlineSessionSearch());
	// session_search_->bIsLanQuery = false;
	session_search_->bIsLanQuery = true; // for LAN testing
	session_search_->MaxSearchResults = 20;
	session_search_->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	dh_on_find_complete = session_interface_->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGI_Zoomies::OnFindComplete));

	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->FindSessions(*local_player->GetPreferredUniqueNetId(), session_search_.ToSharedRef());
	FNetLogger::LogError(TEXT("FindSession_t"));
}

void UGI_Zoomies::OnFindComplete(bool bWasSuccessful)
{
	FNetLogger::LogError(TEXT("OnFindComplete"));
	// unregister the delegate
	session_interface_->ClearOnFindSessionsCompleteDelegate_Handle(
		dh_on_find_complete
		);
	
	// if we found a session, join it
	if (bWasSuccessful && session_search_.IsValid())
	{
		if (session_search_->SearchResults.Num() > 0)
		{
			JoinSession_t(session_search_->SearchResults[0]);
			FNetLogger::LogError(TEXT("FindSession_t[Join]"));
		}
		else
		{
			CreateSession_t();
			FNetLogger::LogError(TEXT("FindSession_t[Create]"));
		}
	}
}

void UGI_Zoomies::CreateSession_t()
{
	FNetLogger::LogError(TEXT("CreateSession_t"));
	session_settings_ = MakeShareable(new FOnlineSessionSettings());
	// session_settings_->bIsLANMatch = false;
	session_settings_->bIsLANMatch = true; // for LAN testing
	session_settings_->NumPublicConnections = 4;
	session_settings_->bShouldAdvertise = true;
	session_settings_->bAllowJoinInProgress = true;
	session_settings_->bUsesPresence = true;
    
	dh_on_create_complete = session_interface_->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onCreateComplete));
    
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->CreateSession(*local_player->GetPreferredUniqueNetId(), NAME_GameSession, *session_settings_);
}

void UGI_Zoomies::onCreateComplete(FName session_name, bool bWasSuccessful)
{
	// unregister the delegate
	session_interface_->ClearOnCreateSessionCompleteDelegate_Handle(
		dh_on_create_complete
		);
	
	// if the session was created successfully, travel to the lobby level
	if (bWasSuccessful)
	{
		// travel to lobby level
		FNetLogger::LogError(TEXT("onCreateComplete"));
		GetWorld()->ServerTravel(TEXT("matchLobby?listen"), true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
	}
}

void UGI_Zoomies::JoinSession_t(const FOnlineSessionSearchResult& search_result)
{
	dh_on_join_complete = session_interface_->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onJoinComplete));
    
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->JoinSession(*local_player->GetPreferredUniqueNetId(), NAME_GameSession, search_result);
}

void UGI_Zoomies::onJoinComplete(FName session_name, EOnJoinSessionCompleteResult::Type result)
{
	// unregister the delegate
	session_interface_->ClearOnJoinSessionCompleteDelegate_Handle(
		dh_on_join_complete
		);
	
	// if the join was successful, travel to the target map
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		APlayerController* player_controller = GetWorld()->GetFirstPlayerController();
		FString travel_url;
		if (session_interface_->GetResolvedConnectString(session_name, travel_url))
		{
			UWorld* world = GetWorld();
			if (world && world->GetMapName().Contains(travel_url))
			{
				//logging
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						30.f,
						FColor::Cyan,
						FString::Printf(TEXT("join complete & already in target map")));
				}
			}
			else
			{
				player_controller->ClientTravel(travel_url, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get travel URL"));
		}
	}
}

void UGI_Zoomies::onDestroySessionComplete(FName session_name, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session %s was destroyed. Success: %s"), *session_name.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"))
	
	if (bWasSuccessful)
	{
		OnHostDisconnected();
	}
}

void UGI_Zoomies::CheckSteamInit()
{
	if (count < max_count && (!is_steamAPI_init || !is_online_session_steam_init))
	{
		InitSteamAPI();
		count+= 1;
		GetWorld()->GetTimerManager().SetTimer(
			UnusedHandle,
			this, &UGI_Zoomies::CheckSteamInit,
			2.0f,
			false);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(UnusedHandle);
	}
	if (count >= max_count)
	{
		UE_LOG(LogTemp, Error, TEXT("SteamAPI init failed max attempts reached"));
	}
}

void UGI_Zoomies::InitSteamAPI()
{
	if (!is_steamAPI_init && SteamAPI_Init())
	{
		is_steamAPI_init = true;
		CSteamID steam_id = SteamUser()->GetSteamID();
		FString steam_username = UTF8_TO_TCHAR(SteamFriends()->GetPersonaName());
		if (steam_id.IsValid() && !steam_username.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("SteamAPI INIT SUCCESS || ID: %llu, Username: %s"), steam_id.ConvertToUint64(), *steam_username);
			SteamNetworkingUtils()->InitRelayNetworkAccess();
		}
	}
	
	if (!is_steamAPI_init)
	{
		UE_LOG(LogTemp, Log, TEXT("SteamAPI init failed"));
		//logging on Screen if GEngine is available
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				30.f,
				FColor::Red,
				FString::Printf(TEXT("SteamAPI init failed")));
		}
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this, &UGI_Zoomies::InitOnlineSubsystemSteam,
			0.5f, false);
	}
}

void UGI_Zoomies::InitOnlineSubsystemSteam()
{
	if (!is_online_session_steam_init)
	{
		online_subsystem_ = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (online_subsystem_)
		{
			session_interface_ = online_subsystem_->GetSessionInterface();
			is_online_session_steam_init = true;
			if (session_interface_.IsValid())
			{
				UE_LOG(
						LogTemp,
						Log,
						TEXT("Online Subsystem steam SUCCESS || SubsystemName: %s"),
						*online_subsystem_->GetSubsystemName().ToString());
			}
		}
	}
	if (!is_online_session_steam_init)
	{
		//logging on Screen if GEngine is available
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				30.f,
				FColor::Red,
				FString::Printf(TEXT("Online Subsystem steam init failed")));
		}
		UE_LOG(LogTemp, Log, TEXT("Online Subsystem steam init failed"));
	}
}