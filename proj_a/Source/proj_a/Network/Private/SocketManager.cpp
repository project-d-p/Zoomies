// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketManager.h"

#include "FNetLogger.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
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
	if (UdpSendTask && UdpSendTask->isRun())
	{
		UdpSendTask->Stop();
		delete UdpSendTask;
		UdpSendTask = nullptr;
	}
	if (UdpSock)
	{
		UdpSock->Close();
		SockSubSystem->DestroySocket(UdpSock);
		UdpSock = nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("UdpSock is destroyed."));
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
	
	UdpSock = FUdpSocketBuilder(TEXT("UdpClientSocket"));
	UdpSock->SetNonBlocking(true);
	return true;
}

bool FSocketManager::RunTask()
{
	UdpSendTask = new FUdpSendTask();
	
	TSharedRef<FInternetAddr> Addr = SockSubSystem->CreateInternetAddr();
	FSocket* UdpSocket = GetUDPSocket();
	if (UdpSocket == nullptr)
	{
		FNetLogger::LogError(TEXT("UDP Socket is null!"));
		return false;
	}
	UdpReceiveTask = new FUdpReceiveTask(Addr->ToString(false), Addr->GetPort());
	return true;
}

FSocket* FSocketManager::GetUDPSocket() const
{
	return UdpSock;
}
