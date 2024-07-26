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
	virtual ~ADPPlayerController() override;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetSeamlessTravelActorList(bool bToTransitionMap, TArray<AActor*>& ActorList) override;
	virtual void AcknowledgePossession(APawn* P) override;
	
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);

	/* Switch level component Called By GameMode & GameState */
	// UFUNCTION(Client, Reliable)
	void SwitchLevelComponent(ELevelComponentType Type);

	/* Get Level Component */
	UBaseLevelComponent* GetLevelComponent() const;

	UPlayerScoreComp* GetScoreManagerComponent() const;
	UPrivateScoreManager* GetPrivateScoreManagerComponent() const;
	UClientSocket* GetClientSocket() const;

	void ReleaseMemory();

	UFUNCTION(Client, Reliable)
	void ClientDestroySession();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	void DeactiveCurrentComponent();
	void ActivateComponent(ELevelComponentType Type);

	// Move To PlayerState
	UPROPERTY(VisibleAnywhere)
	UPrivateScoreManager* PrivateScoreManager;
	UPROPERTY()
	UClientSocket* Socket = nullptr;

	UPROPERTY()
	TMap<uint32, UBaseLevelComponent*> LevelComponents;

	UPROPERTY()
	UBaseLevelComponent* ActiveComponent;
};
