#include "NetworkFailureManager.h"

#include "FNetLogger.h"

void UNetworkFailureManager::Init(IOnlineSessionPtr session_interface)
{
	this->SessionInterface = session_interface;

	FNetLogger::EditerLog(FColor::Green, TEXT("NetworkFailureManager::Init"));
	if (GEngine)
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("NetworkFailureManager::Init Inside"));
		GEngine->OnNetworkFailure().AddUObject(this, &UNetworkFailureManager::HandleNetworkFailure);
	}
}

void UNetworkFailureManager::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
	const FString& String)
{
	UE_LOG(LogTemp, Warning, TEXT("Network Failure: %s"), *String);

	if (Arg == ENetworkFailure::ConnectionLost || Arg == ENetworkFailure::ConnectionTimeout)
	{
		OnHostMigrationDelegate.Broadcast(World, DataManager);
	}
}
