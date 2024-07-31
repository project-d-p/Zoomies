#include "GI_Zoomies.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "steam_api.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

void UGI_Zoomies::Init()
{
	Super::Init();
	CheckSteamInit();
}

// matching session Functions
void UGI_Zoomies::StartMatchMaking()
{
	FindSession();
}

IOnlineSessionPtr UGI_Zoomies::GetOnlineSessionInterface() const
{
	return session_interface_;
}

void UGI_Zoomies::FindSession()
{
	FNetLogger::LogError(TEXT("FindSession_t"));
    
	// Session search settings 
	session_search_ = MakeShareable(new FOnlineSessionSearch());
    
	// Align with CreateSession settings
	bool bIsOnline = true;  // Set true for online mode, false for LAN mode
	session_search_->bIsLanQuery = !bIsOnline;
    
	session_search_->MaxSearchResults = 20;
    
	// Set query settings based on the online/LAN mode
	if (bIsOnline)
	{
		session_search_->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		session_search_->QuerySettings.Set(SETTING_MAPNAME, FString("matchLobby"), EOnlineComparisonOp::Equals);
        
		// If using lobbies (as set in CreateSession)
		session_search_->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}
	else
	{
		// For LAN, we don't need to set SEARCH_LOBBIES
		session_search_->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		session_search_->QuerySettings.Set(SETTING_MAPNAME, FString("matchLobby"), EOnlineComparisonOp::Equals);
	}

	// Add delegate
	dh_on_find_complete = session_interface_->AddOnFindSessionsCompleteDelegate_Handle(
	   FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGI_Zoomies::OnFindComplete));

	// Find sessions
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->FindSessions(*local_player->GetPreferredUniqueNetId(), session_search_.ToSharedRef());
}

void UGI_Zoomies::OnFindComplete(bool bWasSuccessful)
{
	FNetLogger::LogError(TEXT("OnFindComplete"));
	// unregister the delegate
	session_interface_->ClearOnFindSessionsCompleteDelegate_Handle(
		dh_on_find_complete
		);

	//if we found right session, initiate join
	if (bWasSuccessful && session_search_.IsValid())
	{
		if (session_search_->SearchResults.Num() > 0)
		{	
			JoinSessionBySearchResult(session_search_->SearchResults[0]);
			FNetLogger::LogError(TEXT("FindSession_t[Join]"));
		}
		else
		{
			CreateSession();
			FNetLogger::LogError(TEXT("FindSession_t[Create]"));
		}
	}
}

void UGI_Zoomies::CreateSession()
{
	FNetLogger::LogError(TEXT("CreateSession_t"));
	session_settings_ = MakeShareable(new FOnlineSessionSettings());

	
	// 이미 세션이 존재한다면 기존 세션을 삭제한다
	auto ExistingSession = session_interface_->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		session_interface_->DestroySession(NAME_GameSession);

		FNetLogger::LogError(TEXT("Destroy existing session: %s"), NAME_GameSession);
	}
	
	// Online or LAN setting
	bool bIsOnline = true;  // Set true for online mode, false for LAN mode
	session_settings_->bIsLANMatch = !bIsOnline;
    
	session_settings_->NumPublicConnections = 4; // Number of players
	session_settings_->bShouldAdvertise = true; // Advertise the session to others
	session_settings_->bAllowJoinInProgress = true; // Allow joining in progress
	session_settings_->bAllowJoinViaPresence = true; // Allow joining via presence (show sessions to players in current regions)

	// Presence and lobby settings
	session_settings_->bUsesPresence = true; // Use presence for the session
	session_settings_->bUseLobbiesIfAvailable = bIsOnline; // Use lobbies if available
    
	// Steam-related settings (for dedicated server)
	// if (bIsOnline && !session_settings_->bUsesPresence)
	// {
	// 	session_settings_->Set(STEAMPRODUCTNAME, FString("YourAppID"), EOnlineDataAdvertisementType::ViaOnlineService);
	// 	session_settings_->Set(STEAMGAMEDIR, FString("YourGameDir"), EOnlineDataAdvertisementType::ViaOnlineService);
	// 	session_settings_->Set(STEAMGAMEDESC, FString("Your Game Description"), EOnlineDataAdvertisementType::ViaOnlineService);
	// }

	session_settings_->Set(SETTING_MAPNAME, FString("matchLobby"), EOnlineDataAdvertisementType::ViaOnlineService);
    
	// Add delegate
	dh_on_create_complete = session_interface_->AddOnCreateSessionCompleteDelegate_Handle(
	   FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onCreateComplete));
    
	// Create session
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

void UGI_Zoomies::JoinSessionBySearchResult(const FOnlineSessionSearchResult& search_result)
{
	dh_on_join_complete = session_interface_->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onJoinComplete));
    
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->JoinSession(*local_player->GetPreferredUniqueNetId(), NAME_GameSession, search_result);
}

void UGI_Zoomies::onJoinComplete(FName session_name, EOnJoinSessionCompleteResult::Type result)
{
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		APlayerController* player_controller = GetWorld()->GetFirstPlayerController();
		if (player_controller)
		{
			FString connect_string;
			if (session_interface_->GetResolvedConnectString(session_name, connect_string))
			{
				// Get map name from session settings
				FString map_name;
				FOnlineSessionSettings* session_settings = session_interface_->GetSessionSettings(session_name);
				if (session_settings && session_settings->Get(SETTING_MAPNAME, map_name))
				{
					// URL: "IP:Port/MapName"
					FString travel_url = FString::Printf(TEXT("%s/%s"), *connect_string, *map_name);
					FNetLogger::LogError(TEXT("Joining address: %s"), *travel_url);
                    
					// ClientTravel to the map
					player_controller->ClientTravel(travel_url, ETravelType::TRAVEL_Absolute);
				}
				else
				{
					FNetLogger::LogError(TEXT("Failed to get map name from session settings"));
				}
			}
			else
			{
				FNetLogger::LogError(TEXT("Failed to get resolved connect string"));
			}
		}
		else
		{
			FNetLogger::LogError(TEXT("PlayerController is null"));
		}
	}
	else
	{
		FNetLogger::LogError(TEXT("Join session failed with result: %d"), static_cast<int32>(result));
	}

	session_interface_->ClearOnJoinSessionCompleteDelegate_Handle(dh_on_join_complete);
}

void UGI_Zoomies::OnSessionFailure()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->GetNetMode() == NM_ListenServer)
		{
			IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
			if (OnlineSubsystem)
			{
				IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
				if (Sessions.IsValid())
				{
					FNamedOnlineSession* Session = Sessions->GetNamedSession(NAME_GameSession);
					if (Session)
					{
						Sessions->EndSession(NAME_GameSession);
					}
				}
			}
			for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
			{
				APlayerController* PlayerController = It->Get();
				if (PlayerController)
				{
					PlayerController->ClientTravel(TEXT("lobbyLevel?closed"), ETravelType::TRAVEL_Absolute);
				}
			}
			World->ServerTravel(TEXT("lobbyLevel?closed"), true);
			session_interface_->DestroySession(NAME_GameSession);
		}
		else
		{
			APlayerController* CurrentPlayerController = GetWorld()->GetFirstPlayerController();
			if (CurrentPlayerController)
			{
				CurrentPlayerController->ClientTravel(TEXT("lobbyLevel?closed"), ETravelType::TRAVEL_Absolute);
				session_interface_->DestroySession(NAME_GameSession);
			}
		}
	}
}

void UGI_Zoomies::OnDestroyComplete(FName session_name, bool bWasSuccessful)
{
	session_interface_->ClearOnDestroySessionCompleteDelegate_Handle(dh_on_destroy_complete);

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session destroyed successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy session"));
	}
}

bool UGI_Zoomies::ResetSession()
{
	if (session_interface_.IsValid())
	{
		if (session_interface_->GetNamedSession(NAME_GameSession) != nullptr)
		{
			// Register OnDestroySessionComplete delegate
			dh_on_destroy_complete = session_interface_->AddOnDestroySessionCompleteDelegate_Handle(
				FOnDestroySessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::OnDestroyComplete)
			);
			// Destroy the current session
			session_interface_->DestroySession(NAME_GameSession);
			return true;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("no existing session to reset"));
	return false;
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