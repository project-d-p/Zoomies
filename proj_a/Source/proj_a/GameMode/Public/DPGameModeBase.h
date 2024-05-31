// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* newPlayer) override;
	ADPGameModeBase();
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(NetMulticast, Reliable)
	void BroadcastChatMessage(const FString& SenderName, const FString& Message);
private:
	void DisableReplicationForCharacters();
	FTimerHandle TimerHandle_DisableReplication;
};
