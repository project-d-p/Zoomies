#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "TextureTransferManager.h"
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

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* MatchLobbyWidget;
	
	void UpdateUIVisibility() const;
	void ShowUI_ESC() const;
	void getMatchLobbyUI();
	void RemoveMatchLobbyUI();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ACineCameraActor* FixedCamera = nullptr;
	void FindCineCamera();
	
	
	UTextureTransferManager* GetTextureTransferManager() const { return TextureTransferManager; }
private:
	UPROPERTY()
	UTextureTransferManager* TextureTransferManager = nullptr;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	ULC_MatchLobby* LevelComponent = nullptr;
};
