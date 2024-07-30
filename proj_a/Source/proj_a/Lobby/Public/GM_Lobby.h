// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_Lobby.generated.h"

UCLASS()
class PROJ_A_API AGM_Lobby : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGM_Lobby();
	virtual void BeginPlay() override;	
};
