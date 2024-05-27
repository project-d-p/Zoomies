// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"

void UMyGameInstance::StartMatchMaking() {
    UE_LOG(LogTemp, Log, TEXT("START MATCHING"));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Cyan,
            FString::Printf(TEXT("START MATCHING IN MATCHMAKING")));
    }

    //this->FindSession();
}

void UMyGameInstance::FindSession() {

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Cyan,
            FString::Printf(TEXT("I'M HERE IN FIND SESSION")));
    }

    session_search_ = MakeShareable(new FOnlineSessionSearch());
    session_search_->bIsLanQuery = false;
    session_search_->MaxSearchResults = 100000;

    OnFindSessionsCompleteDelegateHandle = match_session_->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnFindSessionsComplete));

    session_search_->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
    //match_session_->FindSessions(0, session_search_.ToSharedRef());
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful) {
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Cyan,
            FString::Printf(TEXT("FOUND HERE")));
    }

    match_session_->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    if (bWasSuccessful && session_search_->SearchResults.Num() > 0) {
        //this->JoinSessionGame(session_search_->SearchResults[0]);
    }
    else {
        //this->CreateSession();
    }
}

void UMyGameInstance::CreateSession() {
    FOnlineSessionSettings session_settings;
    session_settings.bIsLANMatch = false;
    session_settings.NumPublicConnections = 4;
    session_settings.bShouldAdvertise = true;
    session_settings.bAllowJoinInProgress = true;
    session_settings.bUsesPresence = true;

    OnCreateSessionCompleteDelegateHandle = match_session_->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnCreateSessionComplete)
    );
    //match_session_->CreateSession(0, *desired_session_name_, session_settings);
}

void UMyGameInstance::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful) {
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Cyan,
            FString::Printf(TEXT("CREATING SESSION")));
    }
    match_session_->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
    if (bWasSuccessful) {
        UE_LOG(LogTemp, Log, TEXT("Create Session Successfully"));
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("Create Session Failed"));
    }
}

void UMyGameInstance::JoinSessionGame(const FOnlineSessionSearchResult& search_result) {
    OnJoinSessionCompleteDelegateHandle = match_session_->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::OnJoinSessionComplete)
    );
    match_session_->JoinSession(0, *desired_session_name_, search_result);
}

void UMyGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type Result) {
    match_session_->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Cyan,
            FString::Printf(TEXT("JOINED HERE")));
    }
    if (Result == EOnJoinSessionCompleteResult::Success) {
        UE_LOG(LogTemp, Log, TEXT("Join Sesison Success"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Join Session Failed"));
    }
}

void UMyGameInstance::Init() {
	Super::Init();

    online_subsystem_ = IOnlineSubsystem::Get();
    if (online_subsystem_)
    {
        match_session_ = online_subsystem_->GetSessionInterface();
        if (!match_session_.IsValid()) {
            UE_LOG(LogTemp, Warning, TEXT("session interface failed"));
            return;
        }
        FName SubsystemName = online_subsystem_->GetSubsystemName();
        UE_LOG(LogTemp, Log, TEXT("Current Online Subsystem: %s"), *SubsystemName.ToString());
    }

    //if (SteamAPI_Init())
    //{
    //    CSteamID steamID = SteamUser()->GetSteamID();
    //    FString steamUsername = UTF8_TO_TCHAR(SteamFriends()->GetPersonaName());
    //    UE_LOG(LogTemp, Log, TEXT("Steam ID: %llu, Username: %s"), steamID.ConvertToUint64(), *steamUsername);
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Log, TEXT("STEAM INIT FAILED"));

    //    // 추가 디버깅 정보 출력
    //    FString FilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("steam_appid.txt"));
    //    FString FileContent;
    //    if (FFileHelper::LoadFileToString(FileContent, *FilePath))
    //    {
    //        UE_LOG(LogTemp, Log, TEXT("Loaded steam_appid.txt content: %s"), *FileContent);
    //    }
    //    else
    //    {
    //        UE_LOG(LogTemp, Log, TEXT("Failed to load steam_appid.txt"));
    //    }
    //}

	this->is_server_ = false;
    this->desired_session_name_ = "deulee_server";
	//FString LobbyLevelName = TEXT("lobbyLevel");
	//UGameplayStatics::OpenLevel(GetWorld(), *LobbyLevelName);
}

bool UMyGameInstance::IsServer() {
	return this->is_server_;
}