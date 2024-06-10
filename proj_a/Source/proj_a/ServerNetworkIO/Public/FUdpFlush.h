#pragma once

#include "CoreMinimal.h"
#include "message.pb.h"
#include <vector>
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

class FListenSocketRunnable;

class FUdpFlushRunnable : public FRunnable
{
public:
	explicit FUdpFlushRunnable(FListenSocketRunnable* listen_socket_runnable);
	virtual uint32 Run() override;
private:
	void BroadCastMessage(const Message& msg) const;
private:
	FListenSocketRunnable* listen_socket_runnable_;
};