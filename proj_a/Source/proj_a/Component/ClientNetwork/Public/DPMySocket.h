#pragma once

#include "CoreMinimal.h"
#include "DoubleBuffer.h"
#include "DPMySocket.generated.h"

UCLASS()
class PROJ_A_API UMySocket : public UActorComponent
{
	GENERATED_BODY()
public:
	void CreateSocket();
	void Connect(FString ip, uint32 port);
	void RunTask();
	void SendPacket(const Message& msg);
	virtual ~UMySocket() override;
private:
	FSocket* tcp_socket_ = nullptr;
	FRunnableThread* tcp_read_thread_ = nullptr;
	FRunnableThread* tcp_write_thread_ = nullptr;
	DoubleBuffer tcp_write_buffer_;
};
