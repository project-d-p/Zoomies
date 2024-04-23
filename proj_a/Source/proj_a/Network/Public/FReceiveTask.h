#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);

class FReceiveTask : public FRunnable
{
private:
	FSocket	*sock_;
	FRunnableThread* thread_;
	FThreadSafeBool bShouldRun_;

public:
	FReceiveTask(bool bUseTCP);
	virtual ~FReceiveTask();
	virtual uint32 Run() override;
	virtual void Stop() override;
};
