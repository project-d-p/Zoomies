// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "FNetLogger.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
// #include "isteamnetworkingutils.h"

void UMyGameInstance::Init() {
	Super::Init();

    online_subsystem_ = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
    // online_subsystem_ = IOnlineSubsystem::Get();
    if (online_subsystem_)
    {
        match_session_ = online_subsystem_->GetSessionInterface();
        if (!match_session_.IsValid()) {
            UE_LOG(LogTemp, Warning, TEXT("session interface failed"));
            return;
        }
        FName SubsystemName = online_subsystem_->GetSubsystemName();
        UE_LOG(LogTemp, Log, TEXT("Current Online Subsystem: %s"), *SubsystemName.ToString());
        FNetLogger::EditerLog(FColor::Cyan, TEXT("Online Subsystem: %s"), *SubsystemName.ToString());
    }

    if (SteamAPI_Init())
    {
        CSteamID steamID = SteamUser()->GetSteamID();
        FString steamUsername = UTF8_TO_TCHAR(SteamFriends()->GetPersonaName());
        UE_LOG(LogTemp, Log, TEXT("Steam ID: %llu, Username: %s"), steamID.ConvertToUint64(), *steamUsername);
        FNetLogger::EditerLog(FColor::Cyan, TEXT("Steam ID: %llu, Username: %s"), steamID.ConvertToUint64(), *steamUsername);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("STEAM INIT FAILED"));

        FString FilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("steam_appid.txt"));
        FString FileContent;
        if (FFileHelper::LoadFileToString(FileContent, *FilePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Loaded steam_appid.txt content: %s"), *FileContent);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Failed to load steam_appid.txt"));
        }
    }
    this->desired_session_name_ = "zoomies_lobby";
    SteamNetworkingUtils()->InitRelayNetworkAccess();
}

void UMyGameInstance::StartMatchMaking() {
    this->FindSession();
}

void UMyGameInstance::FindSession() {

    FNetLogger::EditerLog(FColor::Cyan, TEXT("FINDING SESSION"));

    session_search_ = MakeShareable(new FOnlineSessionSearch());
    session_search_->bIsLanQuery = true;
    session_search_->MaxSearchResults = 20;

    OnFindSessionsCompleteDelegateHandle = match_session_->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnFindSessionsComplete));

    session_search_->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
    const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    match_session_->FindSessions(*localPlayer->GetPreferredUniqueNetId(), session_search_.ToSharedRef());
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful) {
    match_session_->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    if (bWasSuccessful && session_search_->SearchResults.Num() > 0) {
        FNetLogger::EditerLog(FColor::Cyan, TEXT("LOGME HERE JOIN"));
        
        UE_LOG(LogTemp, Log, TEXT("NUM ROOMS: %d"), session_search_->SearchResults.Num());
        this->JoinSessionGame(session_search_->SearchResults[0]);
    }
    else {
        FNetLogger::EditerLog(FColor::Cyan, TEXT("LOGME HERE CREATE"));

        this->CreateSession();
    }
}

void UMyGameInstance::CreateSession() {
    session_settings_ = MakeShareable(new FOnlineSessionSettings());
    session_settings_->bIsLANMatch = true;
    session_settings_->NumPublicConnections = 4;
    session_settings_->bShouldAdvertise = true;
    session_settings_->bAllowJoinInProgress = true;
    session_settings_->bUsesPresence = true;


    session_settings_->Set(SETTING_MAPNAME, FString("lobbyLevel"), EOnlineDataAdvertisementType::ViaOnlineService);

    OnCreateSessionCompleteDelegateHandle = match_session_->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnCreateSessionComplete)
    );
    const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    match_session_->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *session_settings_);
}

void UMyGameInstance::JoinLobby() {
    GetWorld()->ServerTravel(TEXT("lobbyLevel?listen"), true);
}

void UMyGameInstance::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful) {
    FNetLogger::EditerLog(FColor::Cyan, TEXT("CREATING SESSION"));

    match_session_->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
    if (bWasSuccessful) {
        UE_LOG(LogTemp, Log, TEXT("Create Session Successfully"));
        FString ConnectString;
        match_session_->GetResolvedConnectString(sessionName, ConnectString);
        OnCreateSessionCompleteEvent.Broadcast(*ConnectString, bWasSuccessful);
        JoinLobby();
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("Create Session Failed"));
    }
}

void UMyGameInstance::JoinSessionGame(const FOnlineSessionSearchResult& search_result) {
    OnJoinSessionCompleteDelegateHandle = match_session_->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnJoinSessionComplete)
    );
    const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    match_session_->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, search_result);
}

void UMyGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type Result) {
    match_session_->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
    FNetLogger::EditerLog(FColor::Cyan, TEXT("JOINED HERE"));
    
    if (Result == EOnJoinSessionCompleteResult::Success) {
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

        FString TravelURL;
        if (match_session_->GetResolvedConnectString(sessionName, TravelURL))
        {
            UWorld* World = GetWorld();
            if (World && World->GetMapName().Contains(TravelURL))
            {
                UE_LOG(LogTemp, Log, TEXT("Already in the target map."));
                return;
            }
            PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
            UE_LOG(LogTemp, Log, TEXT("Join Sesison Success"));
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Join Session Failed"));
    }
}

