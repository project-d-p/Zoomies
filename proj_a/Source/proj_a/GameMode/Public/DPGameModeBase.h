// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreManager.h"
#include "ServerChatManager.h"
#include "GameFramework/GameModeBase.h"
#include "ListenSocket.h"
#include <queue>
#include "message.pb.h"
#include "ServerMessageHandler.h"
#include "MessageQueueFilter.h"
#include "DPPlayerController.h"
#include "ServerTimerManager.h"
#include "DPInGameState.h"
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

	void SendChatToAllClients(const FString& SenderName, const FString& Message);

	/** 테스트를 위해서 임시로 public에 선언 */
	UPROPERTY()
	UScoreManagerComp* ScoreManager;

	// Called when the game starts or when spawned
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called every frame
	virtual void Tick(float delta_time) override;

	// Destructor
	virtual ~ADPGameModeBase() override;

private:
	// Implementations
	void SyncMovement();
	void ProcessData(float delta_time);
	void MergeMessages();
	
private:
	// Member variables
	enum { NUM_OF_MAX_CLIENTS = 2 };
	FListenSocketRunnable* listen_socket_ = nullptr;
	bool b_is_game_started = false;
	float time_accumulator_ = 0.0f;
	FMessageQueue_T message_queue_;
	std::map<std::string, ADPPlayerController*> player_controllers_;
	ServerMessageHandler message_handler_;
	
private:
	// Time Manager
	UPROPERTY()
	UServerTimerManager* TimerManager;
	UPROPERTY()
	UServerChatManager* ChatManager;
};
