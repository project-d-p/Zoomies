// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TYPE_MatchingLobby.generated.h"

#define MAX_USERS 4
#define GAME_APPID "480"

USTRUCT(BlueprintType)
struct FLobbyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	bool bIsReady = false;

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	FString Name = "test";
	
	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	APlayerController *PC = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	APlayerState *PS = nullptr;
};

USTRUCT(BlueprintType)
struct FCurrentSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString IPAddress;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 Port;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString SessionID;
};

UCLASS()
class PROJ_A_API ATYPE_MatchingLobby : public AActor
{
	GENERATED_BODY()
};
