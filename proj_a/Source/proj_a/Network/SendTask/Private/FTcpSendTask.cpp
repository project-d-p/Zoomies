#include "FTcpSendTask.h"

#include "SocketManager.h"
#include "FNetLogger.h"

FTcpSendTask::FTcpSendTask()
{
	USocketManager* SocketManager = USocketManager::GetInstance();
	Sock = SocketManager->GetTCPSocket();
	Thread = FRunnableThread::Create(this, TEXT("NetworkTaskThread"), 0, TPri_BelowNormal);
	if (!Thread)
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to create TCP thread"));
	}
}

uint32 FTcpSendTask::Run()
{
	// XXX: 할 일 생기면 추가
	while (true)
	{
		;
	}
	return 0;
}
