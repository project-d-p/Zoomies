#pragma once

#include "CoreMinimal.h"
#include "message.pb.h"
#include <vector>

#include "DoubleBuffer.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

class FListenSocketRunnable;

class FUdpFlushRunnable : public FRunnable
{
public:
	explicit FUdpFlushRunnable(FListenSocketRunnable* listen_socket_runnable);
	void PushUdpFlushMessage(const Message& msg);
	void Flush();
	virtual uint32 Run() override;
private:
	void BroadCastMessage(const Message& msg) const;
	DoubleBuffer udp_send_buffer_;
private:
	FListenSocketRunnable* listen_socket_runnable_;
};