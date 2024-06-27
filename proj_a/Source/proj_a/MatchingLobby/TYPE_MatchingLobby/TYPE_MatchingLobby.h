// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "proj_a/MatchingLobby/PS_MatchingLobby/PS_MatchingLobby.h"
#include "TYPE_MatchingLobby.generated.h"

#define MAX_USERS 2

USTRUCT(BlueprintType)
struct FLobbyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	bool bIsReady = false;

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	FString Name = "test";

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	int64 Avatar = 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	APC_MatchingLobby *PC = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	APS_MatchingLobby *PS = nullptr;
};

UCLASS()
class PROJ_A_API ATYPE_MatchingLobby : public AActor
{
	GENERATED_BODY()
};
