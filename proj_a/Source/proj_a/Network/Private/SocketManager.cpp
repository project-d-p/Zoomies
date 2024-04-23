// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketManager.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "FNetworkTask.h"

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

bool USocketManager::connect(const FString &sIp, int32 port)
{
	FIPv4Address ip;
	FIPv4Address::Parse(sIp, ip);

	TSharedRef<FInternetAddr> addr = socketSubSystem_->CreateInternetAddr();
	addr->SetIp(ip.Value);
	// XXX: 포트 분리
	addr->SetPort(port);

	tcpSock_ = socketSubSystem_->CreateSocket(NAME_Stream, TEXT("InGame_TCP_Socket"), false);
	udpSock_ = socketSubSystem_->CreateSocket(NAME_DGram, TEXT("InGame_UDP_Socket"), false);
	
	return tcpSock_->Connect(*addr);
}

bool USocketManager::runTask()
{
	TCPtask = new FNetworkTask(true);
	// UDP 추가, 실패 경우 추가

	TCPReceiveTask = new FReceiveTask(true);
	
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

// XXX: sock에 tcp, udp를 구분하여 입력.
int32 USocketManager::send(FSocket* &sock, const uint8* data, int32 count)
{
	int32 sent = 0;
	return sock->Send(data, count, sent);
}

int32 USocketManager::receive(FSocket* &sock, uint8& outData, int32 bufferSize)
{
	int32 read = 0;
	return sock->Recv(&outData, bufferSize, read);
}

// XXX: 게임 종료시 호출.
void USocketManager::close() const
{
	TCPtask->Stop();
	TCPReceiveTask->Stop();
	
	udpSock_->Close();
	tcpSock_->Close();
	socketSubSystem_->DestroySocket(udpSock_);
	socketSubSystem_->DestroySocket(tcpSock_);
}
