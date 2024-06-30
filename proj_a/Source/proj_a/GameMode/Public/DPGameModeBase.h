// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreManager.h"
#include "ServerChatManager.h"
#include "GameFramework/GameModeBase.h"
#include <queue>
#include "message.pb.h"
#include "ServerMessageHandler.h"
#include "DPPlayerController.h"
#include "ServerTimerManager.h"
#include "MonsterFactory.h"
#include "SteamNetworkingSocket.h"
#include "DPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	// typedef std::priority_queue<Message, std::vector<Message>, MessageQueueFilter> FMessageQueue_T;
	typedef std::queue<Message> FMessageQueue_T;
	// Sets default values for this character's properties
	ADPGameModeBase();
	void SpawnAndPossessAI();

	void SendChatToAllClients(const FString& SenderName, const FString& Message);

	/** 테스트를 위해서 임시로 public에 선언 */
	UPROPERTY()
	UScoreManagerComp* ScoreManager;
	FTimerHandle TimerHandle_SpawnAI;

	// Called when the game starts or when spawned
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called every frame
	virtual void Tick(float delta_time) override;

	// Destructor
	virtual ~ADPGameModeBase() override;

private:
	// Implementations
	void SyncMovement();
	void SimulateGunFire();
	void SyncHostAiming();
	void ProcessData(float delta_time);
	
private:
	// Member variables
	enum { NUM_OF_MAX_CLIENTS = 2 };
	// FListenSocketRunnable* listen_socket_ = nullptr;

	SteamNetworkingSocket* steam_listen_socket_ = nullptr;
	// HSteamListenSocket steam_listen_socket_;
	
	float time_accumulator_ = 0.0f;
	int gun_fire_ = 0;
	FMessageQueue_T message_queue_;
	std::map<std::string, ADPPlayerController*> player_controllers_;
	ServerMessageHandler message_handler_;
	
private:
	UPROPERTY()
	UServerTimerManager* TimerManager;
	UPROPERTY()
	UServerChatManager* ChatManager;
	UPROPERTY()
	UMonsterFactory* MonsterFactory;
};
