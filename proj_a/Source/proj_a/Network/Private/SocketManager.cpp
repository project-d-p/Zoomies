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

FSocketManager& FSocketManager::GetInstance()
{
	static FSocketManager Instance;
	return Instance;
}

FSocketManager::FSocketManager()
{
	
}

FSocketManager::~FSocketManager()
{

}

void FSocketManager::Close()
{
	UE_LOG(LogTemp, Warning, TEXT("SocketManager start destroying."));
	if (UdpReceiveTask && UdpReceiveTask->isRun())
	{
		UdpReceiveTask->Stop();
		delete UdpReceiveTask;
		UdpReceiveTask = nullptr;
	}
	if (TcpReceiveTask && TcpReceiveTask->isRun())
	{
		TcpReceiveTask->Stop();
		delete TcpReceiveTask;
		TcpReceiveTask = nullptr;
	}
	if (UdpSendTask && UdpSendTask->isRun())
	{
		UdpSendTask->Stop();
		delete UdpSendTask;
		UdpSendTask = nullptr;
	}
	if (TcpSendTask && TcpSendTask->isRun())
	{
		TcpSendTask->Stop();
		delete TcpSendTask;
		TcpSendTask = nullptr;
	}
	if (UdpSock)
	{
		UdpSock->Close();
		SockSubSystem->DestroySocket(UdpSock);
		UdpSock = nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("UdpSock is destroyed."));
	if (TcpSock)
	{
		TcpSock->Close();
		SockSubSystem->DestroySocket(TcpSock);
		TcpSock = nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("TcpSock is destroyed."));
	UE_LOG(LogTemp, Warning, TEXT("SocketManager is destroyed."));
}


bool FSocketManager::Connect(const FString& tIP, int32 tPort)
{
	FIPv4Address ip;
	SockSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> addr = SockSubSystem->CreateInternetAddr();
	
	FIPv4Address::Parse(tIP, ip);
	addr->SetIp(ip.Value);
	addr->SetPort(tPort);
	TcpSock = SockSubSystem->CreateSocket(NAME_Stream, TEXT("InGame_TCP_Socket"), false);
	TcpSock->SetNonBlocking(true);
	
	if (TcpSock->Connect(*addr))
	{
		UdpSock = FUdpSocketBuilder(TEXT("UdpClientSocket"));
		UdpSock->SetNonBlocking(true);
		return true;
	}
	FNetLogger::GetInstance().LogError(TEXT("Failed to connect to server.(TCP)"));
	return false;
}

bool FSocketManager::RunTask()
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

FSocket* FSocketManager::GetUDPSocket() const
{
	return UdpSock;
}

FSocket* FSocketManager::GetTCPSocket() const
{
	return TcpSock;
}
