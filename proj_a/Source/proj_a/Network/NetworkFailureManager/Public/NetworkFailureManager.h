#pragma once

#include "CoreMinimal.h"
#include "DataManager.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetworkFailureManager.generated.h"

DECLARE_EVENT_TwoParams(UNetworkFailureManager, FOnHostMigration, UWorld*, UDataManager*);

UCLASS()
class UNetworkFailureManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(IOnlineSessionPtr session_interface);
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);
	FOnHostMigration& OnHostMigration() { return OnHostMigrationDelegate; }

private:
	IOnlineSessionPtr SessionInterface;
	UPROPERTY()
	UDataManager* DataManager;
	FOnHostMigration OnHostMigrationDelegate;
};
