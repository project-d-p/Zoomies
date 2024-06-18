#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API APC_MatchingLobby : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetReady(bool pIsReady);

	UFUNCTION(BlueprintCallable, Category = "MatchingLobby")
	void ToggleReadyState();
	APlayerController *PC = this; 
protected:
	virtual void BeginPlay() override;
private:
	bool bIsReady = false;
};
