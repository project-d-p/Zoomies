#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"
#include "PC_MatchingLobby.generated.h"

class ULC_MatchLobby;

UCLASS()
class PROJ_A_API APC_MatchingLobby : public APlayerController
{
	GENERATED_BODY()

public:
	APC_MatchingLobby();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetReady(bool pIsReady);
	virtual void AcknowledgePossession(APawn* P) override;


	UFUNCTION(BlueprintCallable, Category = "MatchingLobby")
	void ToggleReadyState();
	void SetCineCameraView();

	UPROPERTY(BlueprintReadWrite, Category = "MatchingLobbyInfo")
	bool bIsReady = false;
	UPROPERTY(BlueprintReadWrite, Category = "MatchingLobbyInfo")
	FString UserName = "DefaultName";

	void ActivateCurrentComponent(APC_MatchingLobby* LocalPlayerController);
	void DeactiveCurrentComponent();
	UUserWidget* GetWidgetByName(UUserWidget* ParentWidget, const FString& WidgetName);
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	ULC_MatchLobby* LevelComponent;
};
