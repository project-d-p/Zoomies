// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketManager.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "FTcpSendTask.h"
#include "FUdpSendTask.h"
#include "Common/UdpSocketBuilder.h"

USocketManager* USocketManager::instance_ = nullptr;

USocketManager* USocketManager::getInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = NewObject<USocketManager>();
		instance_->AddToRoot();
	}
	return instance_;
}

USocketManager::USocketManager()
{
	udpSock_ = nullptr;
	tcpSock_ = nullptr;
	socketSubSystem_ = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
}

bool USocketManager::connect(const FString& tIP, int32 tPort, const FString& uIP, int32 uPort)
{
	FIPv4Address ip;
	TSharedRef<FInternetAddr> addr = socketSubSystem_->CreateInternetAddr();
	
	FIPv4Address::Parse(tIP, ip);
	addr->SetIp(ip.Value);
	addr->SetPort(tPort);
	tcpSock_ = socketSubSystem_->CreateSocket(NAME_Stream, TEXT("InGame_TCP_Socket"), false);

	if (tcpSock_->Connect(*addr))
	{
		udpSock_ = FUdpSocketBuilder(TEXT("UDPClientSocket"))
			.AsNonBlocking()
			.AsReusable()
			.WithBroadcast();

		return true;
	}
	return false;
}

bool USocketManager::runTask()
{
	TCPtask = new FTcpSendTask();
	UDPtask = new FUdpSendTask();
	// UDP 추가, 실패 경우 추가

	// TCPReceiveTask = new FReceiveTask(true);
	UDPReceiveTask = new FReceiveTask(false, getTCPSocket()->GetPortNo());
	
	return true;
}

FSocket* USocketManager::getUDPSocket() const
{
	return udpSock_;
}

FSocket* USocketManager::getTCPSocket() const
{
	return tcpSock_;
}

// XXX: 게임 종료시 호출.
void USocketManager::close() const
{
	TCPReceiveTask->Stop();
	
	tcpSock_->Close();
	socketSubSystem_->DestroySocket(tcpSock_);
}
