#pragma once

#include "CoreMinimal.h"
#include "DoubleBuffer.h"
#include "steamnetworkingtypes.h"
#include "isteamnetworkingsockets.h"
#include "isteamnetworkingutils.h"
#include "ClientSocket.generated.h"

UCLASS()
class PROJ_A_API UClientSocket : public UActorComponent, public FRunnable
{
	GENERATED_BODY()
public:
	UClientSocket();
	virtual ~UClientSocket() override;
	void Connect(const char* ip, uint16 port);
	void RunTask();
	void AsyncSendPacket(const Message& msg);
	virtual uint32 Run() override;
	void Stop();
	void DestoryInstance();

	STEAM_CALLBACK(UClientSocket, OnSteamNetConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
private:
	void HandleRecieveMessages();
	void HandleSendMessages();
	SteamNetworkingConfigValue_t opt_;
	SteamNetworkingIdentity server_identity_;
	int virtual_port;
	HSteamNetConnection connection_;
	DoubleBuffer recieve_buffer_;
	DoubleBuffer send_buffer_;
	FRunnableThread* this_thread_ = nullptr;
	bool bStop = false;

	SteamNetworkingIPAddr server_address_;
};
