#pragma once

#include "CoreMinimal.h"
#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "NetworkFailureManager.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ScoreTypes.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"
#include "GI_Zoomies.generated.h"

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
	IOnlineSessionPtr GetOnlineSessionInterface() const;
	IOnlineSubsystem* GetOnlineSubsystemInterface() const;
	UPROPERTY(VisibleAnywhere, Category = "Network")
	FName SessionName = "";

	/* Number of players before Seamless Travel */
	bool ResetSession();
	UFUNCTION(BlueprintCallable, Category = "Network")
	void OnSessionFailure();
	void ChangeJoinInProgress(bool bCond);
	void AddBanPlayer(const FString& String);

	void ReadFriendList();
	void LoadFriendsList();
	UFUNCTION(BlueprintCallable, Category = "Network")
	void InviteFriendToGame(FString FriendId);
	
	TArray<TSharedRef<FOnlineFriend>> FriendsList;
	UPROPERTY(BlueprintReadOnly, Category = "Friends")
	TArray<FFriendInfo> FriendsArray;
	void ShowLoadingScreen();
	
	int player_count = 0;
	UPROPERTY()
	UNetworkFailureManager* network_failure_manager_;
private:
	// Online subsystem & session interface pointers
	IOnlineSubsystem* online_subsystem_;
	IOnlineSessionPtr session_interface_;

	// search setting & session setting pointers
	TSharedPtr<FOnlineSessionSearch> session_search_;
	TSharedPtr<FOnlineSessionSettings> session_settings_;
	
	// Matchmaking functions & callbacks & handlers
	void FindSession();
	void OnFindComplete(bool bWasSuccessful);
	FDelegateHandle dh_on_find_complete;
	bool JoinSessionBySearchResult(const FOnlineSessionSearchResult& search_result);
	void onJoinComplete(FName session_name, EOnJoinSessionCompleteResult::Type result);
	FDelegateHandle dh_on_join_complete;
	void CreateSession();
	void onCreateComplete(FName session_name, bool bWasSuccessful);
	FDelegateHandle dh_on_create_complete;
	FOnCreateSessionComplete on_create_complete_event;
	
	void OnDestroyComplete(FName session_name, bool bWasSuccessful);
	FDelegateHandle dh_on_destroy_complete;

	int count = 0;
	int max_count = 5;
	bool is_steamAPI_init = false;
	bool is_online_session_steam_init=false;
	bool bIsOnline = false;

	FTimerHandle UnusedHandle;
	void CheckSteamInit();
	void InitSteamAPI();
	void InitOnlineSubsystemSteam();
	bool CheckValidation() const;
	void OnInviteAccepted(const bool bWasSuccessful, const int32 LocalPlayerNum, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

	UPROPERTY()
	UUserWidget* LoadingWidget;
};
