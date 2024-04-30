// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FBaseSendTask.h"
#include "FTcpReceiveTask.h"
#include "FUdpReceiveTask.h"
#include "SocketSubsystem.h"

class PROJ_A_API FSocketManager
{
public:
	static FSocketManager* GetInstance();
	
	FSocketManager();
	~FSocketManager();
	bool Connect(const FString& tIP, int32 tPort);
	bool RunTask();

	FSocket* GetUDPSocket() const;
	FSocket* GetTCPSocket() const;

	/* TCP, UDP sender task */
	FBaseSendTask* TcpSendTask = nullptr;
	FBaseSendTask* UdpSendTask = nullptr;
	//
	/* TCP, UDP receiver task */
	FTcpReceiveTask* TcpReceiveTask = nullptr;
	FUdpReceiveTask* UdpReceiveTask	= nullptr;
	//
	ISocketSubsystem* SockSubSystem = nullptr;
private:
	static FSocketManager* Instance;
	//
	FSocket* UdpSock = nullptr;
	FSocket* TcpSock = nullptr;
	// FSocket* chatSock_;
};
