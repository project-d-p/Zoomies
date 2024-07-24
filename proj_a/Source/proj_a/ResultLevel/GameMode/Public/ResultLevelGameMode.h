#pragma once

#include "CoreMinimal.h"
#include "DPPlayerController.h"
#include "GameFramework/GameMode.h"
#include "ResultLevelGameMode.generated.h"

UCLASS()
class PROJ_A_API AResultLevelGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AResultLevelGameMode();

	virtual ~AResultLevelGameMode() override;

	/* Seamless Travel : Reuse PlayerControllers */
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	virtual void PostSeamlessTravel() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Broadcast(AActor* Sender, const FString& Msg, FName Type) override;
	
protected:
	virtual void Logout(AController* Exiting) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void SpawnNewPlayerPawn(AController* PC);
};
