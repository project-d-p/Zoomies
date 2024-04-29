// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketManager.h"

#include "FNetLogger.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "FTcpSendTask.h"
#include "FUdpReceiveTask.h"
#include "FUdpSendTask.h"
#include "Common/UdpSocketBuilder.h"

USocketManager* USocketManager::Instance = nullptr;

USocketManager* USocketManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<USocketManager>();
	}
	return Instance;
}

USocketManager::USocketManager()
{
	SockSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
}

bool USocketManager::Connect(const FString& tIP, int32 tPort)
{
	FIPv4Address ip;
	TSharedRef<FInternetAddr> addr = SockSubSystem->CreateInternetAddr();

	FIPv4Address::Parse(tIP, ip);
	addr->SetIp(ip.Value);
	addr->SetPort(tPort);
	TcpSock = SockSubSystem->CreateSocket(NAME_Stream, TEXT("InGame_TCP_Socket"), false);

	if (TcpSock->Connect(*addr))
	{
		UdpSock = FUdpSocketBuilder(TEXT("UdpClientSocket"));
		return true;
	}
	FNetLogger::GetInstance().LogError(TEXT("Failed to connect to server.(TCP)"));
	return false;
}

bool USocketManager::RunTask()
{
	TcpSendTask = new FTcpSendTask();
	if (TcpSendTask->isRun())
		UdpSendTask = new FUdpSendTask();

	// TCPReceiveTask = new FReceiveTask(true);
	TSharedRef<FInternetAddr> Addr = SockSubSystem->CreateInternetAddr();
	GetTCPSocket()->GetAddress(*Addr);
	FSocket* UdpSocket = GetUDPSocket();
	if (UdpSocket == nullptr)
	{
		FNetLogger::GetInstance().LogError(TEXT("UDP Socket is null!"));
		return false;
	}
	UdpReceiveTask = new FUdpReceiveTask(Addr->ToString(false), Addr->GetPort());

	return true;
}

FSocket* USocketManager::GetUDPSocket() const
{
	return UdpSock;
}

FSocket* USocketManager::GetTCPSocket() const
{
	return TcpSock;
}
