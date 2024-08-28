#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"
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
	void SetCineCameraView();

	UPROPERTY(BlueprintReadWrite, Category = "MatchingLobbyInfo")
	bool bIsReady = false;
	UPROPERTY(BlueprintReadWrite, Category = "MatchingLobbyInfo")
	FString UserName = "DefaultName";
	UFUNCTION(BlueprintCallable, Category = "Steam")

	bool GetIsReady();
protected:
	virtual void BeginPlay() override;
private:
};
