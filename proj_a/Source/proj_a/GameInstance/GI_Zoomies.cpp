#include "GI_Zoomies.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "steam_api.h"
#include "Online/OnlineSessionNames.h"

void UGI_Zoomies::Init()
{
	Super::Init();

	// Get the online subsystem
	online_subsystem_ = IOnlineSubsystem::Get();
	if (online_subsystem_)
	{
		session_interface_ = online_subsystem_->GetSessionInterface();
		if (!session_interface_.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get session interface"));	
			return ;
		}
		UE_LOG(
			LogTemp,
			Log,
			TEXT("Current Online Subsystem: %s"),
			*online_subsystem_->GetSubsystemName().ToString());
	}

	// Initialize Steam API
	if (SteamAPI_Init())
	{
		CSteamID steam_id = SteamUser()->GetSteamID();
		FString steam_username = UTF8_TO_TCHAR(SteamFriends()->GetPersonaName());
		if (steam_id.IsValid() && !steam_username.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("Steam ID: %llu, Username: %s"), steam_id.ConvertToUint64(), *steam_username);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("STEAM INIT FAILED"));
	}
}

// matching session Functions
void UGI_Zoomies::StartMatchMaking()
{
	FindSession_t();
}

void UGI_Zoomies::FindSession_t()
{
	// session search settings	
	session_search_ = MakeShareable(new FOnlineSessionSearch());
	session_search_->bIsLanQuery = true;
		// online testing
		// session_search_->bIsLanQuery = false;
	session_search_->MaxSearchResults = 20;
	session_search_->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	// register find complete delegate
	dh_on_find_complete = session_interface_->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this,
			&UGI_Zoomies::OnFindComplete
			));

	// start finding sessions
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->FindSessions(
		*local_player->GetPreferredUniqueNetId(),
		session_search_.ToSharedRef());
}

void UGI_Zoomies::OnFindComplete(bool bWasSuccessful)
{
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
		}
		else
		{
			CreateSession_t();
		}
	}
}

void UGI_Zoomies::CreateSession_t()
{
	// session settings
	session_settings_ = MakeShareable(new FOnlineSessionSettings());
	session_settings_->bIsLANMatch = true;
		// online testing
		// session_search_->bIsLANMatch = false;
	session_settings_->NumPublicConnections = 4;
	session_settings_->bShouldAdvertise = true;
	session_settings_->bAllowJoinInProgress = true;
	session_settings_->bUsesPresence = true;
	
	// register create complete delegate
	session_settings_->Set(
		SETTING_MAPNAME,
		FString("matchLobby"),
		EOnlineDataAdvertisementType::ViaOnlineService);
	dh_on_create_complete = session_interface_->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(
			this,
			&UGI_Zoomies::onCreateComplete
			));
	
	// create session
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->CreateSession(
		*local_player->GetPreferredUniqueNetId(),
		NAME_GameSession,
		*session_settings_);
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
		GetWorld()->ServerTravel(TEXT("matchLobby?listen"), true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
	}
}

void UGI_Zoomies::JoinSession_t(const FOnlineSessionSearchResult& search_result)
{
	// register join complete delegate
	dh_on_join_complete = session_interface_->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(
			this,
			&UGI_Zoomies::onJoinComplete
			));
	
	// join session
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->JoinSession(
		*local_player->GetPreferredUniqueNetId(),
		NAME_GameSession,
		search_result);
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