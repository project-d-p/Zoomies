// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ServerTimerManager.h"
#include "DPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ADPGameModeBase();
private:
	UPROPERTY()
	UServerTimerManager* TimerManager;
public:
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
