// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Steam/steam_api.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category="Network")
	void StartMatchMaking();


private:
	void FindSession();
	void OnFindSessionsComplete(bool bWasSuccessful);
	void CreateSession();
	void OnCreateSessionComplete(FName sessionName, bool bWasSucessful);
	void JoinSessionGame(const FOnlineSessionSearchResult& search_result);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type Result);
	bool IsServer();
	

	TSharedPtr<FOnlineSessionSearch> session_search_;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	HSteamListenSocket listen_socket_;
	IOnlineSubsystem* online_subsystem_;
	IOnlineSessionPtr match_session_;
	FString desired_session_name_;
	bool is_server_;
};
