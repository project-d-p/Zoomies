#include "GI_Zoomies.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "steam_api.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Online.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"

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

IOnlineSubsystem* UGI_Zoomies::GetOnlineSubsystemInterface() const
{
	return online_subsystem_;
}

void UGI_Zoomies::FindSession()
{
	// session search settings	
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
	// session create 시
	session_settings_->bIsLANMatch = false;
	session_settings_->NumPublicConnections = 4; // Number of players
	session_settings_->bShouldAdvertise = true; // Advertise the session to others
	session_settings_->bAllowJoinInProgress = true; // Allow joining in progress
	session_settings_->bAllowJoinViaPresence = true; // Allow joining via presence (show sessions to players in current regions)
	// Presence and lobby settings
	session_settings_->bUsesPresence = true; // Use presence for the session
	session_settings_->bUseLobbiesIfAvailable = true; // Use lobbies if available

	SessionName = FName(*FString::Printf(TEXT("GameSession_%s"), *FGuid::NewGuid().ToString()));
	
	// add delegate to handle the result of created session
	session_settings_->Set(
		SETTING_MAPNAME,
		FString("matchLobby"),
		EOnlineDataAdvertisementType::ViaOnlineService);
	dh_on_create_complete = session_interface_->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGI_Zoomies::onCreateComplete));
    
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

void UGI_Zoomies::JoinSessionBySearchResult(const FOnlineSessionSearchResult& search_result)
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
			if (world && !(world->GetMapName().Contains(travel_url)))
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
	else
	{
		ReadFriendList();
	}
}

void UGI_Zoomies::ShowSteamInviteOverlay()
{
	LogFriendsNicknames();
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
				bool IsOnline = Friend->GetPresence().bIsOnline;
				NewFriend.FriendNickname = FText::FromString(FriendNickname);
				NewFriend.FriendId = FriendId->ToString();
				NewFriend.IsOnline = IsOnline;
				FriendsArray.Add(NewFriend);
			}
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

void UGI_Zoomies::LogFriendsNicknames()
{
	for (int32 i = 0; i < FriendsArray.Num(); ++i)
	{
		const FFriendInfo& Friend = FriendsArray[i];
		FString Message = FString::Printf(TEXT("Friend Nickname: %s, Friend ID: %s"),
										  *Friend.FriendNickname.ToString(), *Friend.FriendId);
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
	}
}

void UGI_Zoomies::InviteFriendToGame(FString FriendId)
{
	if (session_interface_.IsValid())
	{
		IOnlineIdentityPtr IdentityInterface = online_subsystem_->GetIdentityInterface();
		FUniqueNetIdPtr uniqueNetId = IdentityInterface->CreateUniquePlayerId(FriendId);
		// 초대 보내기
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

/*void UGI_Zoomies::makeFriendList()
{
	IOnlineFriendsPtr Friends = online_subsystem_->GetFriendsInterface();
	if (Friends.IsValid())
	{
		FString ListName = TEXT("Default");

		if (Friends->GetFriendsList(0, ListName, FriendsList))
		{
			static ConstructorHelpers::FClassFinder<UUserWidget> FriendListWidgetClassFinder(TEXT("/Game/widget/WBP_MatchLobby/WBP_FriendList.WBP_FriendList_C"));
			if (FriendListWidgetClassFinder.Succeeded())
			{
				FriendListWidget = CreateWidget<UUserWidget>(GetWorld(), FriendListWidgetClassFinder.Class);
				if (FriendListWidget)
				{
					UScrollBox* ScrollBox = Cast<UScrollBox>(FriendListWidget->GetWidgetFromName(TEXT("SB_FriendList")));
					if (ScrollBox)
					{
						for (auto& Friend : FriendsList)
						{
							FString FriendNickname = Friend->GetDisplayName();

							static ConstructorHelpers::FClassFinder<UUserWidget> SteamIDWidgetClassFinder(TEXT("/Game/widget/WBP_MatchLobbyt/WBP_SteamID.WBP_SteamID_C"));
							if (SteamIDWidgetClassFinder.Succeeded())
							{
								// WBP_SteamID load
								UUserWidget* SteamIDWidget = CreateWidget<UUserWidget>(GetWorld(), SteamIDWidgetClassFinder.Class);
								if (SteamIDWidget)
								{
									// T_FriendName set Text
									UTextBlock* FriendNameTextBlock = Cast<UTextBlock>(SteamIDWidget->GetWidgetFromName(TEXT("T_FriendName")));
									if (FriendNameTextBlock)
									{
										FriendNameTextBlock->SetText(FText::FromString(FriendNickname));
									}
									// SteamID add child
									ScrollBox->AddChild(SteamIDWidget);
								}
							}
						}
					}
					// set on screen
					FriendListWidget->AddToViewport();
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to get friends list"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to get friends interface"));
	}
}*/
