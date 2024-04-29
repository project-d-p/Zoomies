#pragma once

#include "FBaseReceiveTask.h"
#include "SocketSubsystem.h"

FBaseReceiveTask::~FBaseReceiveTask()
{
	if (Thread)
	{
		Thread->Kill(true);
		delete Thread;
	}
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}

void FBaseReceiveTask::Stop()
{
	ShouldRun = false;
}

void FBaseReceiveTask::Start(const TCHAR* ThreadName)
{
	Thread = FRunnableThread::Create(this, ThreadName, 0, TPri_BelowNormal);
}
