#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "MessageHandler.h"

class FReceiveTask : public FRunnable
{
private:
	FSocket	*sock_;
	int32 Port;
	FRunnableThread* thread_;
	FThreadSafeBool bShouldRun_;
	FMessageHandler MessageHandler;
public:
	FReceiveTask(bool bUseTCP, int32 InPort);
	void EnsureUdpSocketBound(int32 InPort);
	virtual ~FReceiveTask();
	virtual uint32 Run() override;
	virtual void Stop() override;
};
