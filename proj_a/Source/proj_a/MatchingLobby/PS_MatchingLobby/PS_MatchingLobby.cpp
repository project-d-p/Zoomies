// Fill out your copyright notice in the Description page of Project Settings.

#include "PS_MatchingLobby.h"

#include "proj_a/GameInstance/GI_Zoomies.h"

APS_MatchingLobby::APS_MatchingLobby()
{
	SetSessionName();
}

void APS_MatchingLobby::SetSessionName()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		SessionName = GameInstance->GetSessionName();
	}
}

void APS_MatchingLobby::BeginPlay()
{
	Super::BeginPlay();
}
