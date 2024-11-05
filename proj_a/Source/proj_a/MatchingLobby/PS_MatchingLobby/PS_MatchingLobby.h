// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "PS_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API APS_MatchingLobby : public APlayerState
{
	GENERATED_BODY()

protected:
	void SetSessionName();
	virtual void BeginPlay() override;
};