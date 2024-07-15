// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChatManager.h"
#include "ClientSocket.h"
#include "proj_a/Component/InGame/Score/PrivateScoreManager.h"
#include "BaseLevelComponent.h"
#include "ELevelComponentType.h"
#include "MainLevelComponent.h"
#include "DPPlayerController.generated.h"

class UPlayerScoreComp;

UCLASS()
class PROJ_A_API ADPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADPPlayerController();
	
	virtual void Tick(float DeltaSeconds) override;
	
	void SendChatMessageToServer(const FString& Message);
	void ReceiveChatMessage(const FString& SenderName, const FString& Message);
	void InitChatManager(UChatUI* ChatUI);

	/* Switch level component Called By GameMode & GameState */
	void SwitchLevelComponent(ELevelComponentType Type);

	/* Get Level Component */
	UBaseLevelComponent* GetLevelComponent() const;

	UPlayerScoreComp* GetScoreManagerComponent() const;
	UPrivateScoreManager* GetPrivateScoreManagerComponent() const;
	UClientSocket* GetClientSocket() const;

	void ReleaseMemory();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	void DeactiveCurrentComponent();
	void ActivateComponent(ELevelComponentType Type);
	
	UPROPERTY()
	UPrivateScoreManager* PrivateScoreManager;
	UPROPERTY()
	UChatManager* ChatManager = nullptr;
	UPROPERTY()
	UClientSocket* Socket = nullptr;
	UPROPERTY()
	TMap<uint32, UBaseLevelComponent*> LevelComponents;
	UPROPERTY()
	UBaseLevelComponent* ActiveComponent;
};
