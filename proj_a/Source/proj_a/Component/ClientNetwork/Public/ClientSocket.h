#pragma once

#include "CoreMinimal.h"
#include "DoubleBuffer.h"
#include "steamnetworkingtypes.h"
#include "ClientSocket.generated.h"

UCLASS()
class PROJ_A_API UClientSocket : public UActorComponent, public FRunnable
{
	GENERATED_BODY()
public:
	void Connect(const char* ip, uint16 port);
	void RunTask();
	void AsyncSendPacket(const Message& msg);
	virtual uint32 Run() override;
	UClientSocket();
	virtual ~UClientSocket() override;
private:
	void HandleRecieveMessages();
	void HandleSendMessages();
	SteamNetworkingIdentity server_identity_;
	int virtual_port;
	HSteamNetConnection connection_;
	DoubleBuffer recieve_buffer_;
	DoubleBuffer send_buffer_;
	FRunnableThread* this_thread_ = nullptr;

	SteamNetworkingIPAddr server_address_;
};
