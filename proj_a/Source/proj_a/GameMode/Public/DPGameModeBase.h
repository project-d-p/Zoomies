// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreManager.h"
#include "ServerChatManager.h"
#include "GameFramework/GameModeBase.h"
#include "ServerTimerManager.h"
#include "DPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ADPGameModeBase();

	void SendChatToAllClients(const FString& SenderName, const FString& Message);

	/** �׽�Ʈ�� ���ؼ� �ӽ÷� public�� ���� */
	UPROPERTY()
	UScoreManagerComp* ScoreManager;
private:
	UPROPERTY()
	UServerTimerManager* TimerManager;
	UPROPERTY()
	UServerChatManager* ChatManager;
	
public:
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
