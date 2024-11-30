#pragma once

#include "CoreMinimal.h"
#include "CapturedImageWidget.h"
#include "DataManager.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DPLoadingWidget.h"
#include "NetworkFailureManager.generated.h"

DECLARE_EVENT_TwoParams(UNetworkFailureManager, FOnHostMigration, UWorld*, UDataManager*);
DECLARE_EVENT_OneParam(UNetworkFailureManager, FOnSessionDestroyedDelegate, UWorld*);
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
	void ResetInstance();
	void TryReset();
	void Init();
	void SetSessionName(FName Name) { SessionNameGI = Name; }
	void ClearWidget();

	bool bMigrating;
	FName SessionNameGI;
	bool bLoadedJustNow;
	int ExecutionCount = 0;

private:
	void ShowCapturedTextureToPlayer(UTextureRenderTarget2D* CapturedTexture, const TArray<FColor>& Bitmap);
	void DestroyPreviousSession(FOnSessionDestroyedCallback OnSessionDestroyedCallback);
	void OnLevelLoaded(UWorld* LoadedWorld, FOnSessionDestroyedCallback OnSessionDestroyedCallback);
	void CreateNewSession(UWorld* World);
	void CreateSessionComplete(FName SessionName, bool bWasSuccessful, UWorld* World);
	void JoinNewSession(UWorld* World);
	void FindSessionComplete(bool bWasSuccessful, UWorld* World);
	void OnNewLevelLoaded(const FWorldContext& WorldContext, const FString& LevelName);
	void OnNewLevelLoaded(UWorld* World);
	void OnNewLevelLoaded(const FString& LevelName);
	void JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result, UWorld* World);
	void JoinSession(const FOnlineSessionSearchResult& SearchResult, UWorld* World);
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);
	void CreateNewSessionMetaData(::UWorld* World, ::FNamedOnlineSession* CurrentSession, const FUniqueNetIdRef& NewHostPlayerID);
	void CaptureViewport();
	void SaveSessionMetaData(UWorld* World);
	bool ValidateAddr(FString& Addr);

	IOnlineSessionPtr SessionInterface;
	FOnHostMigration OnHostMigrationDelegate;
	FOnSessionDestroyedDelegate OnSessionDestroyedDelegate;
	FOnlineSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FDelegateHandle OnDestroyCompleteDelegateHandle;
	FDelegateHandle OnCreateCompleteDelegateHandle;
	FDelegateHandle OnJoinCompleteDelegateHandle;
	FDelegateHandle OnFindCompleteDelegateHandle;
	bool bNextHost;
	
	FName DesiredSessionName;
	FName DesiredMapName;
	int DesiredMaxPlayers = 0;
	FString SavedBanList;
	FString ExistingPlayersList;

	
	UPROPERTY()
	UDataManager* DataManager;
	UPROPERTY()
	UCapturedImageWidget* CapturedImageWidget;
	UPROPERTY()
	UTexture2D* CapturedTexture2D;
	
	TSubclassOf<UCapturedImageWidget> CapturedImageWidgetClass;
	TSubclassOf<UUserWidget> WidgetLoading;
	UPROPERTY()
	UDPLoadingWidget* LoadingWidgetInstance;
	TSubclassOf<UUserWidget> WidgetHostMigration;
	UPROPERTY()
	UUserWidget* HostMigrationWidgetInstance;
};
