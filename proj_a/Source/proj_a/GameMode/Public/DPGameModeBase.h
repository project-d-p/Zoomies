// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreManager.h"
#include "ServerChatManager.h"
#include "GameFramework/GameModeBase.h"
#include <queue>

#include "ANetworkManager.h"
#include "message.pb.h"
#include "ServerMessageHandler.h"
#include "DPPlayerController.h"
#include "IChatGameMode.h"
#include "ServerTimerManager.h"
#include "MonsterFactory.h"
#include "DPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPGameModeBase : public AGameModeBase, public IChatGameMode
{
	GENERATED_BODY()
public:
	// typedef std::priority_queue<Message, std::vector<Message>, MessageQueueFilter> FMessageQueue_T;
	typedef std::queue<Message> FMessageQueue_T;
	// Sets default values for this character's properties
	ADPGameModeBase();
	void OnGameStart();

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;

	virtual UServerChatManager* GetChatManager() const override { return ChatManager; }
	
	// monster
	enum { NUM_OF_MAX_MONSTERS = 2 };
	std::vector<ABaseMonsterAIController*> monster_controllers_;
	std::vector<int32> empty_monster_slots_;

	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	
	std::condition_variable Condition;
	std::mutex Mutex;
	std::atomic<int32> PendingQueries;
	
	UPROPERTY()
	UScoreManagerComp* ScoreManager;
	FTimerHandle TimerHandle_SpawnAI;

	// Called when the game starts or when spawned
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void StartPlay() override;
	void EndGame();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick(float delta_time) override;

	// Destructor
	virtual ~ADPGameModeBase() override;

	UServerTimerManager* GetTimerManager() const { return TimerManager; }

private:
	// Implementations
	void SyncMovement();
	void SimulateGunFire();
	void SimulateCatch();
	void SimulateAiming();
	void SyncMonsterMovement();
	void ProcessData(float delta_time);
	void MonsterMoveSimulate(float delta_time);
	void SpawnMonsters(float delta_time);

	// Member variables
	enum { NUM_OF_MAX_CLIENTS = 2 };

	// SteamNetworkingSocket* steam_listen_socket_ = nullptr;
	UPROPERTY()
	UANetworkManager* NetworkManager = nullptr;
	
	
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
	bool bStart = false;
	bool bTimeSet = false;
};
