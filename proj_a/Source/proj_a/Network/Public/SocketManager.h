// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FBaseSendTask.h"
#include "FTcpReceiveTask.h"
#include "FUdpReceiveTask.h"
#include "SocketSubsystem.h"
#include "SocketManager.generated.h"

UCLASS()
class PROJ_A_API USocketManager : public UObject
{
	GENERATED_BODY()
public:
	static USocketManager* GetInstance();
	
	USocketManager();
	bool Connect(const FString& tIP, int32 tPort);
	bool RunTask();

	FSocket* GetUDPSocket() const;
	FSocket* GetTCPSocket() const;
	
	void Close() const;

	/* TCP, UDP sender task */
	FBaseSendTask* TcpSendTask = nullptr;
	FBaseSendTask* UdpSendTask = nullptr;
	//
	/* TCP, UDP receiver task */
	FTcpReceiveTask* TcpReceiveTask = nullptr;
	FUdpReceiveTask* UdpReceiveTask	= nullptr;
	//
	ISocketSubsystem* SockSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
private:
	static USocketManager* Instance;
	//
	FSocket* UdpSock = nullptr;
	FSocket* TcpSock = nullptr;
	// FSocket* chatSock_;
};
