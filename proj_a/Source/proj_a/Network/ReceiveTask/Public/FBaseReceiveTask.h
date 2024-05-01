#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"

class FBaseReceiveTask : public FRunnable
{
protected:
	FSocket* Socket;
	FRunnableThread* Thread;
	FThreadSafeBool ShouldRun;

public:
	FBaseReceiveTask() : Socket(nullptr), Thread(nullptr), ShouldRun(true) {}
	virtual ~FBaseReceiveTask() override;
	
	virtual bool isRun() { if (ShouldRun) { return true; } return false; }
	virtual uint32 Run() override = 0;
	void Stop();
	void Start(const TCHAR* ThreadName);
};
