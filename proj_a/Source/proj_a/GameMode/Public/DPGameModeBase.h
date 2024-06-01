// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ListenSocket.h"
#include <queue>
#include <functional>
#include "message.pb.h"
#include "MessageEndpoint.h"
#include "MessageQueueFilter.h"
#include "DPGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class PROJ_A_API ADPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	typedef std::priority_queue<Message, std::vector<Message>, MessageQueueFilter> MessageQueue_t;
public:
	// Sets default values for this character's properties
	ADPGameModeBase();

	// Called when the game starts or when spawned
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called every frame
	virtual void Tick(float delta_time) override;

	// bind for RPC
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Destructor
	virtual ~ADPGameModeBase() override;
private:
	void StartGameLogic(float delta_time);
	void UpdateTime(float delta_time);
	bool IsGameOver() const;
	void MergeMessages();
	
	enum { NUM_OF_MAX_CLIENTS = 2 };
	FListenSocketRunnable* listen_socket_ = nullptr;
	bool b_is_game_started = false;
	float time_accumulator_ = 0.0f;
	MessageQueue_t message_queue_;
private:
	// For Replicated
	UPROPERTY(Replicated)
	int32 remain_time_ = 300;
};
