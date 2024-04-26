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

class FBaseSendTask : public FRunnable
{
protected:
	FSocket* sock_;
	FRunnableThread* thread_;
	FThreadSafeBool bShouldRun_;

public:
	FBaseSendTask() : sock_(nullptr), thread_(nullptr), bShouldRun_(true) {}
	virtual ~FBaseSendTask() { Stop(); if (thread_) { thread_->Kill(true); } }

	virtual uint32 Run() = 0;
	virtual void Stop() { bShouldRun_ = false; }

	// TCP, UDP의 행방 결정시 이동
	static TQueue<FInputData, EQueueMode::Spsc> InputQueue;
	static Movement ProtoData;
};
