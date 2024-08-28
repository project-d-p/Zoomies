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
	return session_interface_;
}

IOnlineSubsystem* UGI_Zoomies::GetOnlineSubsystemInterface() const
{
	return online_subsystem_;
}

void UGI_Zoomies::FindSession()
{
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
	SessionName = FName(*FString::Printf(TEXT("GameSession_%s"), *FGuid::NewGuid().ToString()));
	
	// Create session
	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->CreateSession(*local_player->GetPreferredUniqueNetId(), SessionName, *session_settings_);
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

bool UGI_Zoomies::JoinSessionBySearchResult(const FOnlineSessionSearchResult& search_result)
{
	FString BanList;
	if (search_result.Session.SessionSettings.Get(FName("BanList"), BanList))
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("BanList: %s"), *BanList);
		CSteamID SteamIDRaw = SteamUser()->GetSteamID();
		FString PlayerID = FString::Printf(TEXT("%llu"), SteamIDRaw.ConvertToUint64());
		if (BanList.Contains(PlayerID))
		{
			FNetLogger::EditerLog(FColor::Red, TEXT("You are banned from this session"));
			return false;
		}
	}
	
	dh_on_join_complete = session_interface_->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onJoinComplete));
	SessionName = FName(*search_result.Session.GetSessionIdStr());

	const ULocalPlayer* local_player = GetWorld()->GetFirstLocalPlayerFromController();
	session_interface_->JoinSession(*local_player->GetPreferredUniqueNetId(), SessionName, search_result);
	return true;
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
		// 세션 설정 업데이트
		FOnlineSessionSettings UpdatedSessionSettings = ExistingSession->SessionSettings;
        
		// 세션을 비공개로 설정
		UpdatedSessionSettings.bAllowJoinInProgress = false;
        
		// 추가적으로 검색 결과에서 숨기기 위해
		UpdatedSessionSettings.bShouldAdvertise = false;

		// 세션 설정 업데이트
		SessionInterface->UpdateSession(SessionName, UpdatedSessionSettings);
	}
}

void UGI_Zoomies::AddBanPlayer(const FString& String)
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
		BanList += String + TEXT(",");
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
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("온라인 서브시스템이 초기화되지 않았습니다."));
        }
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
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("유효하지 않은 친구 ID를 건너뜁니다."));
                    }
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
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("친구 리스트를 가져오지 못했습니다."));
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("온라인 친구 인터페이스를 사용할 수 없습니다."));
        }
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
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("친구에게 초대를 보냈습니다."));
			}
			UE_LOG(LogTemp, Log, TEXT("success to send invite"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail to send invite"));
		}
	}
}
