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
	FRunnableThread* thread_;
	FThreadSafeBool bShouldRun_;
	FMessageHandler MessageHandler;
public:
	FReceiveTask(bool bUseTCP);
	virtual ~FReceiveTask();
	virtual uint32 Run() override;
	virtual void Stop() override;
};
