#include "FTcpReceiveTask.h"

#include "Marshaller.h"
#include "SocketManager.h"
#include "MessageHandler.h"
#include "message.pb.h"
#include "FNetLogger.h"

FTcpReceiveTask::FTcpReceiveTask()
{
	if (InitializeSocket())
	{
		Start(TEXT("NetworkReceiverThreadTCP"));
	}
	else 
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to initialize TCP socket"));
	}
}

FTcpReceiveTask::~FTcpReceiveTask()
{
	UE_LOG(LogTemp, Warning, TEXT("TcpReceiveTask is destroyed."));
	if (Thread)
	{
		delete Thread;
		Thread = nullptr;
	}
}


bool FTcpReceiveTask::InitializeSocket()
{
	FSocketManager &sockManager = FSocketManager::GetInstance();
	Socket = sockManager.GetTCPSocket();
	
	return Socket != nullptr;
}

uint32 FTcpReceiveTask::Run()
{
	const int32 bufferSize = 1024;
	TArray<uint8> receiveData;
	
	FMessageHandler msgHandler;

	receiveData.Reserve(bufferSize);
	receiveData.SetNumZeroed(bufferSize);
	while (ShouldRun)
	{
		FNetLogger::GetInstance().LogInfo(TEXT("TcpReceiveTask is running."));
		int32 BytesReceived = 0;
		// XXX: 자주 오지 않는 데이터가 추가될 경우 HasPendingData()를 고려.
		if (Socket->Recv(receiveData.GetData(), bufferSize, BytesReceived))
		{
			if (BytesReceived > 0)
			{
				Message msg = Marshaller::DeserializeMessage(receiveData);
				msgHandler.HandleMessage(msg);
				receiveData.SetNumZeroed(bufferSize);
			}
		}
		FPlatformProcess::Sleep(0.1);
	}
	return 0;
}
