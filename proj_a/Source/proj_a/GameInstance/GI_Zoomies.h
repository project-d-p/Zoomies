// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "GI_Zoomies.generated.h"


USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()

	TArray<TArray<EAnimal>> CapturedAnimals;
	TArray<FScoreData> ScoreDatas;
	bool bIsJob = false;
};

UCLASS()
class PROJ_A_API UGI_Zoomies : public UGameInstance
{
	GENERATED_BODY()
public:
	void OnHostDisconnected();
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);
	virtual void Init() override;

	// Matching starting function
	UFUNCTION(BlueprintCallable, Category = "Network")
	void StartMatchMaking();
	void ResetSession();

private:
	// Online subsystem & session interface pointers
	IOnlineSubsystem* online_subsystem_;
	IOnlineSessionPtr session_interface_;

	// search setting & session setting pointers
	TSharedPtr<FOnlineSessionSearch> session_search_;
	TSharedPtr<FOnlineSessionSettings> session_settings_;
	
	// Matchmaking functions & callbacks & handlers
	void FindSession_t();
	void OnFindComplete(bool bWasSuccessful);
	FDelegateHandle dh_on_find_complete;
	void JoinSession_t(const FOnlineSessionSearchResult& search_result);
	void onJoinComplete(FName session_name, EOnJoinSessionCompleteResult::Type result);
	FDelegateHandle dh_on_join_complete;
	void CreateSession_t();
	void onCreateComplete(FName session_name, bool bWasSuccessful);
	FDelegateHandle dh_on_create_complete;
	FOnCreateSessionComplete on_create_complete_event;
	FDelegateHandle dh_on_session_failure;
	void OnSessionFailure(const FUniqueNetId& NetId, ESessionFailure::Type FailureType);
	void OnDestroyComplete(FName session_name, bool bWasSuccessful);
	FDelegateHandle dh_on_destroy_complete;

	// Param that is used to check if the steamAPI is initialized
	int count = 0;
	int max_count = 5;
	bool is_steamAPI_init = false;
	bool is_online_session_steam_init =false;
	FTimerHandle UnusedHandle;
	void CheckSteamInit();
	void InitSteamAPI();
	void InitOnlineSubsystemSteam();
};
