#pragma once

#include <tiffio.h>

#include "CoreMinimal.h"
#include "DataManager.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetworkFailureManager.generated.h"

DECLARE_EVENT_TwoParams(UNetworkFailureManager, FOnHostMigration, UWorld*, UDataManager*);
typedef void (UNetworkFailureManager::*FOnSessionDestroyedCallback)(UWorld*);

UCLASS()
class UNetworkFailureManager : public UObject
{
	GENERATED_BODY()
	
public:
	UNetworkFailureManager();
	FOnHostMigration& OnHostMigration() { return OnHostMigrationDelegate; }
	UDataManager* GetDataManager() { return DataManager; }
	int GetDesiredMaxPlayers() { return DesiredMaxPlayers; }
	void Init();

private:
	void DestroyPreviousSession(FOnSessionDestroyedCallback OnSessionDestroyedCallback);
	void OnLevelLoaded(UWorld* LoadedWorld, FOnSessionDestroyedCallback OnSessionDestroyedCallback);
	void CreateNewSession(UWorld* World);
	void CreateSessionComplete(FName SessionName, bool bWasSuccessful, UWorld* World);
	void JoinNewSession(UWorld* World);
	void FindSessionComplete(bool bWasSuccessful, UWorld* World);
	void JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result, UWorld* World);
	void JoinSession(const FOnlineSessionSearchResult& SearchResult, UWorld* World);
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);
	void CreateNewSessionMetaData(UWorld* World, const FUniqueNetIdRepl& NewHostPlayerID);
	void SaveSessionMetaData(UWorld* World);
	bool ValidateAddr(FString& Addr);
	
	IOnlineSessionPtr SessionInterface;
	FOnHostMigration OnHostMigrationDelegate;
	FOnlineSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FDelegateHandle OnDestroyCompleteDelegateHandle;
	FDelegateHandle OnCreateCompleteDelegateHandle;
	FDelegateHandle OnJoinCompleteDelegateHandle;
	FDelegateHandle OnFindCompleteDelegateHandle;
	bool bNextHost;
	UPROPERTY()
	UDataManager* DataManager;
	FName DesiredSessionName;
	FName DesiredMapName;
	int DesiredMaxPlayers = 0;
};
