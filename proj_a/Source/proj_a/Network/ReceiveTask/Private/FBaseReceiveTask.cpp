#pragma once

#include "FBaseReceiveTask.h"

FBaseReceiveTask::~FBaseReceiveTask()
{
	if (Thread)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

void FBaseReceiveTask::Stop()
{
	ShouldRun = false;
	Thread->WaitForCompletion();
}

void FBaseReceiveTask::Start(const TCHAR* ThreadName)
{
	Thread = FRunnableThread::Create(this, ThreadName, 0, TPri_BelowNormal);
}
