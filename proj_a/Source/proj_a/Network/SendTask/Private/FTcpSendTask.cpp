#include "FTcpSendTask.h"

#include "SocketManager.h"
#include "FNetLogger.h"

FTcpSendTask::FTcpSendTask()
{
	FSocketManager* SocketManager = FSocketManager::GetInstance();
	Sock = SocketManager->GetTCPSocket();
	Thread = FRunnableThread::Create(this, TEXT("NetworkTaskThread"), 0, TPri_BelowNormal);
	if (!Thread)
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to create TCP thread"));
	}
}

FTcpSendTask::~FTcpSendTask()
{
	UE_LOG(LogTemp, Warning, TEXT("TcpSendTask is destroyed."));
}

uint32 FTcpSendTask::Run()
{
	// XXX: 할 일 생기면 추가
	FNetLogger::GetInstance().LogInfo(TEXT("TcpSendTask is running."));
	while (bShouldRun)
	{
		;
	}
	return 0;
}
