#include "GI_Zoomies.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "steam_api.h"
#include "Kismet/GameplayStatics.h"
#include "CompileMode.h"
#include "NavigationSystemTypes.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Online.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"

void UGI_Zoomies::Init()
{
	Super::Init();
	CheckSteamInit();
	
	network_failure_manager_ = NewObject<UNetworkFailureManager>(this, UNetworkFailureManager::StaticClass());
	network_failure_manager_->Init();
	
#if EDITOR_MODE
	bIsOnline = false;
#elif LAN_MODE
	bIsOnline = false;
#else
	bIsOnline = true;
#endif
}

// matching session Functions
void UGI_Zoomies::StartMatchMaking()
{
	FindSession();
}

IOnlineSessionPtr UGI_Zoomies::GetOnlineSessionInterface() const
{
	if (session_interface_.IsValid())
	{
		return session_interface_;
	}
	UE_LOG(LogTemp, Error, TEXT("Session interface is not valid"));
	return nullptr;
}

IOnlineSubsystem* UGI_Zoomies::GetOnlineSubsystemInterface() const
{
	return online_subsystem_;
}

void UGI_Zoomies::FindSession()
{
	if(!CheckValidation())
	{
		UE_LOG( LogTemp, Error, TEXT("FindSession: Validation failed") );
		return ;
	}
	FNetLogger::LogError(TEXT("FindSession_t"));
    
	// Session search settings 
	session_search_ = MakeShareable(new FOnlineSessionSearch());
    
	// Align with CreateSession settings
	session_search_->bIsLanQuery = !bIsOnline;
	session_search_->MaxSearchResults = 20;
    
	// Set query settings based on the online/LAN mode
	session_search_->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	session_search_->QuerySettings.Set(SETTING_MAPNAME, FString("matchLobby"), EOnlineComparisonOp::Equals);
	if (bIsOnline)
	{
		// If using lobbies (as set in CreateSession)
		session_search_->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
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
	if(!CheckValidation())
	{
		UE_LOG( LogTemp, Error, TEXT("OnFindComplete: Validation failed") );
		return ;
	}
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
			for (int32 i = 0; i < session_search_->SearchResults.Num(); i++)
			{
				if (JoinSessionBySearchResult(session_search_->SearchResults[i]))
					return ;
				FNetLogger::LogError(TEXT("Session[%d]: %s"), i, *session_search_->SearchResults[i].Session.OwningUserName);
			}
			CreateSession();
			FNetLogger::LogError(TEXT("FindSession_t[Create Another Session]"));
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
	if(!CheckValidation())
	{
		UE_LOG( LogTemp, Error, TEXT("CreateSession: Validation failed") );
		return ;
	}
	FNetLogger::LogError(TEXT("CreateSession_t"));
	session_settings_ = MakeShareable(new FOnlineSessionSettings());

	// TODO: Should Not Destory Existing Session Because that will cause the server(playing) to be destroyed
	// If there is an existing session, destroy it
	// auto ExistingSession = session_interface_->GetNamedSession(NAME_GameSession);
	// if (ExistingSession != nullptr)
	// {
	// 	session_interface_->DestroySession(NAME_GameSession);
	//
	// 	FNetLogger::LogError(TEXT("Destroy existing session: %s"), NAME_GameSession);
	// }
	
	// Online or LAN setting
	session_settings_->bIsLANMatch = !bIsOnline;
    
	session_settings_->NumPublicConnections = 4; // Number of players
	session_settings_->bShouldAdvertise = true; // Advertise the session to others
	session_settings_->bAllowJoinInProgress = true; // Allow joining in progress
	session_settings_->bAllowJoinViaPresence = true; // Allow joining via presence (show sessions to players in current regions)
	session_settings_->bUsesPresence = true; // Use presence for the session
	session_settings_->bAllowInvites = true;
	// session_settings_->bAllowJoinViaPresenceFriendsOnly
	
	if (bIsOnline)
	{
		session_settings_->bUseLobbiesIfAvailable = true; // Use lobbies if available
	}
    
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
	// set SessionName to Unique
	SessionName = FName(*FString::Printf(TEXT("Zoomies_%s"), *FGuid::NewGuid().ToString()));
	session_settings_->Set(FName("SESSION_NAME"), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	
	// Create session
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->CreateSession(*local_player->GetPreferredUniqueNetId(), NAME_GameSession, *session_settings_);
}

void UGI_Zoomies::onCreateComplete(FName session_name, bool bWasSuccessful)
{
	if(!CheckValidation())
	{
		UE_LOG( LogTemp, Error, TEXT("onCreateComplete: Validation failed") );
		return ;
	}
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

void UGI_Zoomies::RestrictNewClientAccessAndAllowExistingPlayers()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);

	if (ExistingSession != nullptr)
	{
		FOnlineSessionSettings UpdatedSessionSettings = ExistingSession->SessionSettings;
        
		UpdatedSessionSettings.bAllowJoinInProgress = false;
		UpdatedSessionSettings.bShouldAdvertise = false;

		FString ExistingPlayers;
		for (auto& Player : ExistingSession->RegisteredPlayers)
		{
			ExistingPlayers += Player->ToString() + TEXT(",");
		}
		UpdatedSessionSettings.Set(FName("ExistingPlayersList"), ExistingPlayers, EOnlineDataAdvertisementType::ViaOnlineService);
        
		SessionInterface->UpdateSession(SessionName, UpdatedSessionSettings);

		UE_LOG(LogTemp, Log, TEXT("RestrictNewClientAccessAndAllowExistingPlayers: 기존 플레이어만 접근할 수 있도록 설정되었습니다."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RestrictNewClientAccessAndAllowExistingPlayers: 세션이 존재하지 않습니다."));
	}
}

bool UGI_Zoomies::IsPlayerAllowedToJoin(const FString& PlayerId, const FName& SessionNameToCheck) const
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionNameToCheck);

	if (ExistingSession != nullptr)
	{
		FString BanList;
		if (ExistingSession->SessionSettings.Get(FName("BanList"), BanList))
		{
			FNetLogger::EditerLog(FColor::Red, TEXT("BanList: %s"), *BanList);
			if (BanList.Contains(PlayerId))
			{
				FNetLogger::EditerLog(FColor::Red, TEXT("IsPlayerAllowedToJoin: Player %s is banned from this session."), *PlayerId);
				return false;
			}
		}

		FString ExistingPlayers;
		if (ExistingSession->SessionSettings.Get(FName("ExistingPlayersList"), ExistingPlayers))
		{
			if (ExistingPlayers.Contains(PlayerId))
			{
				UE_LOG(LogTemp, Log, TEXT("IsPlayerAllowedToJoin: Player %s is allowed to join."), *PlayerId);
				return true;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("IsPlayerAllowedToJoin: Player %s is not allowed to join."), *PlayerId);
	return false;
}

bool UGI_Zoomies::JoinSessionBySearchResult(const FOnlineSessionSearchResult& search_result)
{
	if(!CheckValidation())
	{
		UE_LOG( LogTemp, Error, TEXT("JoinSessionBySearchResult: Validation failed") );
		return false;
	}

	FString RetrievedSessionName;
	FName SessionNameToCheck;
	if (search_result.Session.SessionSettings.Get(FName("SESSION_NAME"), RetrievedSessionName))
	{
		SessionNameToCheck = FName(*RetrievedSessionName);
	}
	else
	{
		SessionNameToCheck = FName(*search_result.Session.GetSessionIdStr());
	}
	
	FString PlayerID = GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId()->ToString();
	if (!IsPlayerAllowedToJoin(PlayerID, SessionNameToCheck))
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("JoinSessionBySearchResult: Player is not allowed to join the session."));
		return false;
	}
	
	SessionName = SessionNameToCheck;
	
	dh_on_join_complete = session_interface_->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onJoinComplete));
	
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->JoinSession(*local_player->GetPreferredUniqueNetId(), SessionName, search_result);
	return true;
}

void UGI_Zoomies::OnInviteAccepted(const bool bWasSuccessful, const int32 LocalPlayerNum, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (!CheckValidation())
	{
		UE_LOG(LogTemp, Error, TEXT("OnInviteAccepted: Validation failed"));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("OnInviteAccepted: OnInviteAccepted is called"));

	if (bWasSuccessful && InviteResult.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("OnInviteAccepted: Invite successfully accepted"));
		ShowLoadingScreen();

		dh_on_join_complete = session_interface_->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onJoinComplete));
		
		FString RetrievedSessionName;
		if (InviteResult.Session.SessionSettings.Get(FName("SESSION_NAME"), RetrievedSessionName))
		{
			SessionName = FName(*RetrievedSessionName);
		}
		else
		{
			SessionName = FName(*InviteResult.Session.GetSessionIdStr());
		}

		const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
		session_interface_->JoinSession(*local_player->GetPreferredUniqueNetId(), SessionName, InviteResult);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OnInviteAccepted: Invite was not successful or InviteResult is invalid"));
	}
}


void UGI_Zoomies::onJoinComplete(FName session_name, EOnJoinSessionCompleteResult::Type result)
{
	if(!CheckValidation())
	{
		UE_LOG( LogTemp, Error, TEXT("onJoinComplete: Validation failed") );
		return;
	}
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
	if(!CheckValidation())
	{
		UE_LOG( LogTemp, Error, TEXT("OnSessionFailure: Validation failed") );
		return;
	}
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
					FNamedOnlineSession* Session = Sessions->GetNamedSession(SessionName);
					if (Session)
					{
						Sessions->EndSession(SessionName);
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
			session_interface_->DestroySession(SessionName);
			SessionName = "";
		}
		else
		{
			APlayerController* CurrentPlayerController = GetWorld()->GetFirstPlayerController();
			if (CurrentPlayerController)
			{
				CurrentPlayerController->ClientTravel(TEXT("lobbyLevel?closed"), ETravelType::TRAVEL_Absolute);
				session_interface_->DestroySession(SessionName);
			}
		}
	}
}

void UGI_Zoomies::ChangeJoinInProgress(bool bCond)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);

	if (ExistingSession != nullptr)
	{
		FOnlineSessionSettings UpdatedSessionSettings = ExistingSession->SessionSettings;
        
		UpdatedSessionSettings.bAllowJoinInProgress = false;
        
		UpdatedSessionSettings.bShouldAdvertise = false;

		SessionInterface->UpdateSession(SessionName, UpdatedSessionSettings);
	}
}

void UGI_Zoomies::AddBanPlayer(const FString& PlayerId)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);

	if (ExistingSession != nullptr)
	{
		FOnlineSessionSettings UpdatedSessionSettings = ExistingSession->SessionSettings;

		///
		FString MapName;
		UpdatedSessionSettings.Get(SETTING_MAPNAME, MapName);
		FNetLogger::EditerLog(FColor::Cyan, TEXT("MapName: %s"), *MapName);
		///
		
		FString BanList;
		UpdatedSessionSettings.Get(FName("BanList"), BanList);
		BanList += PlayerId + TEXT(",");
		UpdatedSessionSettings.Set(FName("BanList"), BanList, EOnlineDataAdvertisementType::ViaOnlineService);

		SessionInterface->UpdateSession(SessionName, UpdatedSessionSettings);
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
		if (session_interface_->GetNamedSession(SessionName) != nullptr)
		{
			dh_on_destroy_complete = session_interface_->AddOnDestroySessionCompleteDelegate_Handle(
				FOnDestroySessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::OnDestroyComplete)
			);
			session_interface_->DestroySession(SessionName);
			SessionName = "";
			
			FriendsList.Empty();
			FriendsArray.Empty();
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
#if not (EDITOR_MODE || LAN_MODE)
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
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this, &UGI_Zoomies::InitOnlineSubsystemSteam,
			0.5f, false);
#endif
		InitOnlineSubsystemSteam();
#if not (EDITOR_MODE || LAN_MODE)
	}
#endif
}

void UGI_Zoomies::InitOnlineSubsystemSteam()
{
	if (!is_online_session_steam_init)
	{
		online_subsystem_ = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
#if EDITOR_MODE || LAN_MODE
		online_subsystem_ = IOnlineSubsystem::Get(/*STEAM_SUBSYSTEM*/);
#else
		online_subsystem_ = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
#endif
		if (online_subsystem_)
		{
			session_interface_ = online_subsystem_->GetSessionInterface();
			is_online_session_steam_init = true;
			// network_failure_manager_->Init(session_interface_);
			if (session_interface_.IsValid())
			{
				UE_LOG(
						LogTemp,
						Log,
						TEXT("Online Subsystem steam SUCCESS || SubsystemName: %s"),
						*online_subsystem_->GetSubsystemName().ToString());
				session_interface_->AddOnSessionUserInviteAcceptedDelegate_Handle(
					FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UGI_Zoomies::OnInviteAccepted)
				);
				UE_LOG(LogTemp, Log, TEXT("Online Subsystem steam init success"));
			}
		}
	}
	if (!is_online_session_steam_init)
	{
		UE_LOG(LogTemp, Log, TEXT("Online Subsystem steam init failed"));
	}
	else
	{
		ReadFriendList();
	}
}

void UGI_Zoomies::ReadFriendList()
{
	IOnlineFriendsPtr Friends = online_subsystem_->GetFriendsInterface();
	if (Friends.IsValid())
	{
		Friends->ReadFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default));
	}
}

void UGI_Zoomies::LoadFriendsList()
{
    if (!online_subsystem_)
    {
		UE_LOG(LogTemp, Log, TEXT("online subsystem is not valid"));
        return;
    }

	if (FriendsArray.Num() > 0)
	{
		FriendsArray.Empty();
	}

    IOnlineFriendsPtr Friends = online_subsystem_->GetFriendsInterface();
    if (Friends.IsValid())
    {
        FString ListName = TEXT("Default");

        if (Friends->GetFriendsList(0, ListName, FriendsList))
        {
            for (auto Friend : FriendsList)
            {
                FFriendInfo NewFriend;
                FString FriendNickname = Friend->GetDisplayName();
                FUniqueNetIdPtr FriendId = Friend->GetUserId();

                if (!FriendId.IsValid())
                {
                	
					UE_LOG(LogTemp, Log, TEXT("Invalid Friend ID"));
                    continue;
                }

                NewFriend.FriendNickname = FText::FromString(FriendNickname);
                NewFriend.FriendId = FriendId->ToString();
                NewFriend.IsOnline = Friend->GetPresence().bIsOnline;
                FriendsArray.Add(NewFriend);
            }

            FriendsArray.Sort([](const FFriendInfo& A, const FFriendInfo& B)
            {
                return A.IsOnline && !B.IsOnline;
            });
        }
        else
        {
			UE_LOG(LogTemp, Log, TEXT("failed to get friends list"));
        }
    }
    else
    {
    	UE_LOG(LogTemp, Log, TEXT("online friends interface is not valid"));
    }
}

void UGI_Zoomies::InviteFriendToGame(FString FriendId)
{
	if (session_interface_.IsValid())
	{
		IOnlineIdentityPtr IdentityInterface = online_subsystem_->GetIdentityInterface();
		FUniqueNetIdPtr uniqueNetId = IdentityInterface->CreateUniquePlayerId(FriendId);
		bool bInviteSent = session_interface_->SendSessionInviteToFriend(0, SessionName, *uniqueNetId);
		if (bInviteSent)
		{
			UE_LOG(LogTemp, Log, TEXT("success to send invite"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail to send invite"));
		}
	}
}


bool UGI_Zoomies::CheckValidation() const
{
	if (!session_interface_.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session interface is not valid."));
		return false;
	}
	if (online_subsystem_ == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Online subsystem is not valid."));
		return false;
	}
	return true;
}

void UGI_Zoomies::ShowLoadingScreen()
{
	UClass* WidgetClass = nullptr;
	FString WidgetPath = TEXT("/Game/widget/widget_loading.widget_loading_C");

	WidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *WidgetPath));

	if (WidgetClass != nullptr)
	{
		LoadingWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WidgetClass);

		if (LoadingWidget != nullptr)
		{
			LoadingWidget->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("ShowLoadingScreen::Loading screen displayed"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ShowLoadingScreen::Failed to create widget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowLoadingScreen::Failed to load widget class"));
	}
}

void UGI_Zoomies::HideLoadingScreen()
{
	if (LoadingWidget != nullptr)
	{
		LoadingWidget->RemoveFromParent();
		LoadingWidget = nullptr;
		UE_LOG(LogTemp, Log, TEXT("HideLoadingScreen::Loading screen hidden"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HideLoadingScreen::No loading screen to hide"));
	}
}