#pragma once

#include "CoreMinimal.h"
#include "DPPlayerController.h"
#include "IChatGameMode.h"
#include "ServerChatManager.h"
#include "GameFramework/GameMode.h"
#include "ResultLevelGameMode.generated.h"

UCLASS()
class PROJ_A_API AResultLevelGameMode : public AGameMode, public IChatGameMode
{
	GENERATED_BODY()

public:
	AResultLevelGameMode();

	virtual ~AResultLevelGameMode() override;

	/* Seamless Travel : Reuse PlayerControllers */
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	virtual void PostSeamlessTravel() override;
	UServerChatManager* GetChatManager() const { return ChatManager; }
	
	// UFUNCTION(NetMulticast, Reliable)
	// virtual void Broadcast(AActor* Sender, const FString& Msg, FName Type) override;
	
protected:
	virtual void Logout(AController* Exiting) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void CheckPlayersAllTraveled();
	void SpawnNewPlayerPawn(AController* PC);
	
	UPROPERTY()
	UServerChatManager* ChatManager;
	int CurrentPlayerCount = 0;

	bool bStart = false;
};
