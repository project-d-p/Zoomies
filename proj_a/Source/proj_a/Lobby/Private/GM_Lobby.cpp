// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Lobby.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "proj_a/GameInstance/GI_Zoomies.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"

AGM_Lobby::AGM_Lobby()
{
	PlayerControllerClass = ADPPlayerController::StaticClass();
	DefaultPawnClass = ADPCharacter::StaticClass(); 
}

void AGM_Lobby::BeginPlay()
{
	Super::BeginPlay();

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->ResetSession();
	}
}

