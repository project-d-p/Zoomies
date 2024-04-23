// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FNetworkTask.h"
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
	bool connect(const FString& iP, int32 port);
	bool runTask();

	FSocket* getUDPSocket() const;
	FSocket* getTCPSocket() const;

	// XXX: sock 인자는 tcp, udp를 구분하여 입력. 
	static int32 send(FSocket* &sock, const uint8* data, int32 count);
	static int32 receive(FSocket* &sock, uint8& outData, int32 bufferSize);
	void close() const;

	/* TCP, UDP sender task */
	FNetworkTask *TCPtask;
	FNetworkTask *UDPtask;
	//
	/* TCP, UDP receiver task */
	FReceiveTask *TCPReceiveTask;
	FReceiveTask *UDPReceiveTask;
	//
	
private:
	static USocketManager* instance_;
	FSocket* udpSock_ = nullptr;
	FSocket* tcpSock_ = nullptr;
	// FSocket* chatSock_;
	ISocketSubsystem* socketSubSystem_;
};
