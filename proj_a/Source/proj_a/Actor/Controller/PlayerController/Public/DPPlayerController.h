// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "proj_a/Component/InGame/Score/PrivateScoreManager.h"
#include "BaseLevelComponent.h"
#include "ELevelComponentType.h"
#include "MainLevelComponent.h"
#include "TextureTransferManager.h"
#include "DPPlayerController.generated.h"

class UPlayerScoreComp;

UCLASS()
class PROJ_A_API ADPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADPPlayerController();
	virtual ~ADPPlayerController() override;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetSeamlessTravelActorList(bool bToTransitionMap, TArray<AActor*>& ActorList) override;
	virtual void AcknowledgePossession(APawn* P) override;
	
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);

	/* Switch level component Called By GameMode & GameState */
	void SwitchLevelComponent(ELevelComponentType Type);

	/* Get Level Component */
	UFUNCTION(BlueprintCallable)
	UBaseLevelComponent* GetLevelComponent() const;
	UBaseLevelComponent* GetLevelComponent(ELevelComponentType Type) const;

	template <typename T>
	T* GetLevelComponentAs(const ELevelComponentType E) const
	{
		UBaseLevelComponent* LevelComponent = GetLevelComponent(E);
		return LevelComponent ? Cast<T>(LevelComponent) : nullptr;
	}

	UPlayerScoreComp* GetScoreManagerComponent() const;
	UPrivateScoreManager* GetPrivateScoreManagerComponent() const;
	UTextureTransferManager* GetTextureTransferManager() const { return TextureTransferManager; }

	UANetworkManager* GetNetworkManager() const;
	
	void ReleaseMemory();

	UFUNCTION(Client, Reliable)
	void ClientDestroySession();

	UFUNCTION(Client, Reliable)
	void ConnectToServer(ELevelComponentType Type);
	
	// UPROPERTY(BlueprintReadOnly, Category= "UI")
	// UUserWidget* UIWidget = nullptr;
	UUserWidget* getUIWidget();
	void RemoveUIWidget();
	void ShowUI_ESC();
	
	UFUNCTION()
	void OnPossessEvent(APawn* OldPawn_, APawn* NewPawn);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	void SetLevelComponent();
	void DeactiveCurrentComponent();
	void ActivateComponent(ELevelComponentType Type);
	

	// Move To PlayerState
	UPROPERTY(VisibleAnywhere)
	UPrivateScoreManager* PrivateScoreManager;

	UPROPERTY()
	UANetworkManager* NetworkManager = nullptr;

	UPROPERTY()
	TMap<uint32, UBaseLevelComponent*> LevelComponents;
	UPROPERTY()
	TMap<FString, uint32> LevelEnumMap;

	UPROPERTY()
	UBaseLevelComponent* ActiveComponent = nullptr;

	UPROPERTY()
	UTextureTransferManager* TextureTransferManager = nullptr;
};
