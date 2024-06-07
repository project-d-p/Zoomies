// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class PROJ_A_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* newPlayer) override;
private:
	void CheckAndStartGame();
	void StartGame() const;
};
