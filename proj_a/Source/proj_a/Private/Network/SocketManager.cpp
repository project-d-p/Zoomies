// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketManager.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"

USocketManager* USocketManager::getInstance()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<USocketManager>();
		Instance->AddToRoot();
	}
	return Instance;
}

USocketManager::USocketManager()
{
	sock_ = nullptr;
	socketSubSystem_ = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
}

bool USocketManager::connect(const FString &sIp, int32 port)
{
	FIPv4Address ip;
	FIPv4Address::Parse(sIp, ip);

	TSharedRef<FInternetAddr> addr = socketSubSystem_->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(port);

	sock_ = socketSubSystem_->CreateSocket(NAME_Stream, TEXT("test"), false);

	return sock_->Connect(*addr);
}

int32 USocketManager::send(const uint8* data, int32 count) const
{
	int32 sent = 0;
	return sock_->Send(data, count, sent);
}

int32 USocketManager::receive(uint8& outData, int32 bufferSize) const
{
	int32 read = 0;
	return sock_->Recv(&outData, bufferSize, read);
}

void USocketManager::close() const
{
	sock_->Close();
	socketSubSystem_->DestroySocket(sock_);
}
