// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FBaseSendTask.h"
#include "FReceiveTask.h"
#include "SocketManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API USocketManager : public UObject
{
	GENERATED_BODY()
public:
	static USocketManager* getInstance();
	
	USocketManager();
	bool connect(const FString& tIP, int32 tPort, const FString& uIP, int32 uPort);
	bool runTask();

	FSocket* getUDPSocket() const;
	FSocket* getTCPSocket() const;
	
	void close() const;

	/* TCP, UDP sender task */
	FBaseSendTask *TCPtask;
	FBaseSendTask *UDPtask;
	//
	/* TCP, UDP receiver task */
	FReceiveTask *TCPReceiveTask;
	FReceiveTask *UDPReceiveTask;
	//
	ISocketSubsystem* socketSubSystem_;
	
private:
	static USocketManager* instance_;
	FSocket* udpSock_ = nullptr;
	FSocket* tcpSock_ = nullptr;
	// FSocket* chatSock_;
};
