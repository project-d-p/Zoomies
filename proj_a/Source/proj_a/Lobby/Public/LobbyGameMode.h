// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* newPlayer);
private:
	void CheckAndStartGame();
	void StartGame();
};
