#include "NetworkFailureManager.h"

#include "FNetLogger.h"
#include "GameMapsSettings.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
// #include "Engine/GameMapsSettings.h"
#include "Engine/GameViewportClient.h"
#include "Algo/Sort.h"

UNetworkFailureManager::UNetworkFailureManager()
{
	bNextHost = false;
	DataManager = NewObject<UDataManager>();
}

void UNetworkFailureManager::Init()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(/*STEAM_SUBSYSTEM*/);
	this->SessionInterface = OnlineSubsystem->GetSessionInterface();

	if (GEngine)
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("NetworkFailureManager::Init"));
		GEngine->OnNetworkFailure().AddUObject(this, &UNetworkFailureManager::HandleNetworkFailure);
	}
}

void UNetworkFailureManager::DestroyPreviousSession(FOnSessionDestroyedCallback OnSessionDestroyedCallback)
{
	if (SessionInterface.IsValid())
	{
		OnDestroyCompleteDelegateHandle = SessionInterface->OnDestroySessionCompleteDelegates.AddLambda([this, OnSessionDestroyedCallback](FName SessionName, bool bWasSuccessful)
		{
			this->SessionInterface->OnDestroySessionCompleteDelegates.Remove(this->OnDestroyCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
				if (GameMapsSettings)
				{
					FName DefaultLevel = FName(*GameMapsSettings->GetGameDefaultMap());
					FNetLogger::EditerLog(FColor::Green, TEXT("Default Level: %s"), *DefaultLevel.ToString());
					FNetLogger::LogError(TEXT("Default Level: %s"), *DefaultLevel.ToString());
				
					FString DefaultLevelWithOption = DefaultLevel.ToString() + TEXT("?closed");
					FName NewLevelName = FName(*DefaultLevelWithOption);
				
					FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNetworkFailureManager::OnLevelLoaded, OnSessionDestroyedCallback);
				
					UGameplayStatics::OpenLevel(this, NewLevelName);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session: %s"), *SessionName.ToString());
			}
		});

		SessionInterface->DestroySession(NAME_GameSession);
	}
}

void UNetworkFailureManager::OnLevelLoaded(UWorld* LoadedWorld, FOnSessionDestroyedCallback OnSessionDestroyedCallback)
{
	// 레벨이 완전히 로드된 후 호출
	FNetLogger::EditerLog(FColor::Green, TEXT("Level Loaded Successfully"));
	
	// 델리게이트 해제
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	// 함수 포인터 호출
	(this->*OnSessionDestroyedCallback)(LoadedWorld);
}

void UNetworkFailureManager::CreateNewSession(UWorld* World)
{
	// Online Mode : False
	// SessionSettings.bIsLANMatch = false;
	// Lan Mode : True
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 4; // Number of players
	SessionSettings.bShouldAdvertise = true; // Advertise the session to others
	SessionSettings.bAllowJoinInProgress = true; // Allow joining in progress
	SessionSettings.bAllowJoinViaPresence = true; // Allow joining via presence (show sessions to players in current regions)
	SessionSettings.bUsesPresence = true; // Use presence for the session
	SessionSettings.bAllowInvites = true;
	if (!SessionSettings.bIsLANMatch)
	{
		SessionSettings.bUseLobbiesIfAvailable = true;
	}

	SessionSettings.Set(SETTING_MAPNAME, DesiredMapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("SESSION_NAME"), DesiredSessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	
	OnCreateCompleteDelegateHandle = SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkFailureManager::CreateSessionComplete, World);

	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
}

void UNetworkFailureManager::CreateSessionComplete(FName SessionName, bool bWasSuccessful, UWorld* World)
{
	if (bWasSuccessful)
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("Session Created Successfully"));
		SessionInterface->OnCreateSessionCompleteDelegates.Remove(OnCreateCompleteDelegateHandle);

		FString MapName = DesiredMapName.ToString();
		FString TravelURL = FString::Printf(TEXT("%s?listen"), *MapName);
		World->ServerTravel(TravelURL, true);
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Failed to create session"));
	}
}

void UNetworkFailureManager::JoinNewSession(UWorld* World)
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	// Online Mode : False
	// SessionSearch->bIsLanQuery = false;
	// Lan Mode : True
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SETTING_MAPNAME, DesiredMapName.ToString(), EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName("SESSION_NAME"), DesiredSessionName.ToString(), EOnlineComparisonOp::Equals);
	if (!SessionSearch->bIsLanQuery)
	{
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	OnFindCompleteDelegateHandle = SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetworkFailureManager::FindSessionComplete, World);

	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void UNetworkFailureManager::FindSessionComplete(bool bWasSuccessful, UWorld* World)
{
	SessionInterface->OnFindSessionsCompleteDelegates.Remove(OnFindCompleteDelegateHandle);
	if (bWasSuccessful)
	{
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FString SessionName;
			SearchResult.Session.SessionSettings.Get(FName("SESSION_NAME"), SessionName);
			// 세션 이름 확인
			if (SessionName == DesiredSessionName) // 원하는 세션 이름과 비교
			{
				// 원하는 세션을 찾았으면 그 세션에 접속
				JoinSession(SearchResult, World);
				break;
			}
		}
	}
	else
	{
		FNetLogger::EditerLog(FColor::Green,TEXT("Failed to find session: Retrying..."));
		JoinNewSession(World);
	}
}


void UNetworkFailureManager::JoinSession(const FOnlineSessionSearchResult& SearchResult, UWorld* World)
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Joining Session..."));
	OnJoinCompleteDelegateHandle = SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetworkFailureManager::JoinSessionComplete, World);
	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);
}

void UNetworkFailureManager::JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result, UWorld* World)
{
	SessionInterface->OnJoinSessionCompleteDelegates.Remove(OnJoinCompleteDelegateHandle);
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("Joined Session Successfully"));
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FString ConnectString;
			if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
			{
				if (!ValidateAddr(ConnectString))
				{
					FNetLogger::EditerLog(FColor::Green, TEXT("Address Invalid, Trying Again .."));
					SessionInterface->DestroySession(SessionName);
					JoinNewSession(World);
				}
				FString MapName;
				FOnlineSessionSettings* SessionSettingBySearch = SessionInterface->GetSessionSettings(SessionName);
				if (SessionSettingBySearch && SessionSettingBySearch->Get(SETTING_MAPNAME, MapName))
				{
					FString URL = FString::Printf(TEXT("%s/%s"), *ConnectString, *MapName);
					FNetLogger::EditerLog(FColor::Green, TEXT("ClientTravel URL: %s"), *URL);
					
					PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
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
		FNetLogger::EditerLog(FColor::Red, TEXT("Failed to join session"));
	}
}

void UNetworkFailureManager::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
                                                  const FString& String)
{
	UE_LOG(LogTemp, Warning, TEXT("Network Failure: %s"), *String);
	FNetLogger::EditerLog(FColor::Red, TEXT("Network Failure: %s"), *String);

	if (Arg == ENetworkFailure::ConnectionLost)
	{
		this->SaveSessionMetaData(World);
		if (bNextHost)
		{
			OnHostMigrationDelegate.Broadcast(World, DataManager);
			DestroyPreviousSession(&UNetworkFailureManager::CreateNewSession);
		}
		else
		{
			DestroyPreviousSession(&UNetworkFailureManager::JoinNewSession);
		}
	}
}

void UNetworkFailureManager::CreateNewSessionMetaData(UWorld* World, const FUniqueNetIdRepl& NewHostPlayerID)
{
	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
	if (IdentityInterface.IsValid())
	{
		FString PlayerName = IdentityInterface->GetPlayerNickname(*NewHostPlayerID);

		// Create a new session name based on the player's name
		DesiredSessionName = FName(*FString::Printf(TEXT("Session_%s"), *PlayerName));
		FNetLogger::EditerLog(FColor::Red, TEXT("New Session Name: %s"), *DesiredSessionName.ToString());

		DesiredMapName = FName(*FString::Printf(TEXT("%s"), *World->GetMapName()));
		FNetLogger::EditerLog(FColor::Red, TEXT("New Map Name: %s"), *DesiredMapName.ToString());
	}
}

void UNetworkFailureManager::SaveSessionMetaData(UWorld* World)
{
	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (CurrentSession)
	{
		/* For Debugging */
		if (CurrentSession->RegisteredPlayers.Num() == 2)
		{
			bNextHost = false;
			FNetLogger::EditerLog(FColor::Red, TEXT("You are the Last Player in the Session"));
			return ;
		}
		TArray<FUniqueNetIdRepl> RegisteredPlayers;
		for (int32 i = 0; i < CurrentSession->RegisteredPlayers.Num(); i++)
		{
			IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
			if (IdentityInterface->GetPlayerNickname(*CurrentSession->RegisteredPlayers[i]) == CurrentSession->OwningUserId->ToString())
			{
				continue;
			}
			// Not Include the First Player(Original Host)
			RegisteredPlayers.Add(CurrentSession->RegisteredPlayers[i]);
			FNetLogger::EditerLog(FColor::Blue, TEXT("Player %d: %s"), i, *(IdentityInterface->GetPlayerNickname(*CurrentSession->RegisteredPlayers[i])));
		}
		auto CompareByNickname = [&World](const FUniqueNetIdRepl& A, const FUniqueNetIdRepl& B) -> bool
		{
			IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
    
			if (!A.IsValid() || !B.IsValid() || !IdentityInterface.IsValid())
			{
				return false;
			}
    
			FString NicknameA = IdentityInterface->GetPlayerNickname(*A.GetUniqueNetId());
			FString NicknameB = IdentityInterface->GetPlayerNickname(*B.GetUniqueNetId());

			return NicknameA < NicknameB;  // 문자열을 기준으로 사전순 정렬
		};
		// Algo::Sort(RegisteredPlayers, CompareByNickname);
		RegisteredPlayers.Sort(CompareByNickname);
		for (int32 i = 0; i < RegisteredPlayers.Num(); i++) {
			IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Player %d: %s"), i, *(IdentityInterface->GetPlayerNickname(*RegisteredPlayers[i])));
		}
		const FUniqueNetIdRepl LocalPlayerID = World->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId();
		const FUniqueNetIdRepl NextHostID = RegisteredPlayers[0];
		if (NextHostID.IsValid() && NextHostID == LocalPlayerID)
		{
			bNextHost = true;
		}
		else
		{
			bNextHost = false;
		}
		CreateNewSessionMetaData(World, NextHostID);
		// CreateNewSessionMetaData(World, LocalPlayerID);
	}
	else
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("CurrentSession is null"));
	}
}

bool UNetworkFailureManager::ValidateAddr(FString& Addr)
{
	FString Address;
	FString PortStr;
	if (Addr.Split(":", &Address, &PortStr))
	{
		// 포트를 int32로 변환
		int32 Port = FCString::Atoi(*PortStr);

		// 포트가 0인지 확인
		if (Port == 0)
		{
			FNetLogger::LogError(TEXT("Port is 0, leaving session and retrying"));
			return false; // 포트가 0이면 더 이상 진행하지 않음
		}
	}
	return true;
}
