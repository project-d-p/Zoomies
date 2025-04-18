// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TYPE_MatchingLobby.generated.h"

#define MAX_USERS Zoomies::MAX_PLAYERS
#define GAME_APPID "480"

USTRUCT(BlueprintType)
struct FLobbyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	bool bIsReady = false;

	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	FString Name = "Wait...";
	
	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	APlayerController *PC = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	APlayerState *PS = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "LobbyInfo")
	int32 PlayerId = -1;
};

USTRUCT(BlueprintType)
struct FCurrentSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString IPAddress;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 Port = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString SessionID;
};

USTRUCT(BlueprintType)
struct FFriendInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Friends")
	FText FriendNickname;

	UPROPERTY(BlueprintReadOnly, Category = "Friends")
	FString FriendId;
	
	UPROPERTY(BlueprintReadOnly, Category = "Friends")
	bool IsOnline;
};

UCLASS()
class PROJ_A_API ATYPE_MatchingLobby : public AActor
{
	GENERATED_BODY()
};
