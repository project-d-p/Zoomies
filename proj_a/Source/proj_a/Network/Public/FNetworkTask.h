#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "movement.pb.h"

struct FInputData
{
	FVector2D InputVector;
	int32 Type;
};

class FNetworkTask : public FRunnable
{
private:
	FSocket* sock_;
	FRunnableThread* thread_;
	FThreadSafeBool bShouldRun_;
public:
	FNetworkTask(bool bUseTCP);
	virtual ~FNetworkTask();
	virtual uint32 Run() override;
	virtual void Stop() override;

	static TQueue<FInputData, EQueueMode::Spsc> InputQueue;
	static Movement ProtoData;
};
