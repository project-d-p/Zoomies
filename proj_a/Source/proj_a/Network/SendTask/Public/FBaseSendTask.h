#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "movement.pb.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

struct FInputData
{
	FVector2D InputVector;
	int32 Type;
};

class FBaseSendTask : public FRunnable
{
protected:
	FSocket* Sock;
	FRunnableThread* Thread;
	FThreadSafeBool bShouldRun;

public:
	FBaseSendTask() : Sock(nullptr), Thread(nullptr), bShouldRun(true) {}
	virtual ~FBaseSendTask()
	{
		if (Thread)
		{
			delete Thread;
			Thread = nullptr;
		}
	}

	virtual uint32 Run() = 0;
	virtual bool isRun() { if (bShouldRun) { return true; } return false; }
	virtual void Stop() { bShouldRun = false; Thread->WaitForCompletion(); }

	// TCP, UDP의 행방 결정시 이동
	static TQueue<FInputData, EQueueMode::Spsc> InputQueue;
	static Movement ProtoData;
};
