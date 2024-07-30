// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "steam_api.h"
#include "proj_a/MatchingLobby/TYPE_MatchingLobby/TYPE_MatchingLobby.h"
#include "SteamInvite.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API USteamInvite : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Steam")
	void ShowSteamOverlayInviteDialog();
	void InitializeInviteCallback();
	void ConfigureSteamSessionInvite();
private:
	FString InviteURL;
	FCurrentSessionInfo CurrentSessionInfo;
	static FCurrentSessionInfo SetSessionInfoByCurrentSession();
};
