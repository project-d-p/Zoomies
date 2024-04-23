#include "FReceiveTask.h"
#include "SocketManager.h"
#include "Marshaller.h"
#include "Sockets.h"
#include "message.pb.h"

FReceiveTask::FReceiveTask(bool bUseTCP)
{
	USocketManager* SocketManager = USocketManager::getInstance();
	sock_ = bUseTCP ? SocketManager->getTCPSocket() : SocketManager->getUDPSocket();
	bShouldRun_ = true;
	thread_ = FRunnableThread::Create(this, TEXT("NetworkReceiverThread"), 0, TPri_BelowNormal);
}

FReceiveTask::~FReceiveTask()
{
	if (thread_)
	{
		thread_->Kill(true);
		delete thread_;
	}
}

uint32 FReceiveTask::Run()
{
	const int32 BufferSize = 1024;
	TArray<uint8> ReceivedData;
	ReceivedData.SetNumZeroed(BufferSize);
	while (bShouldRun_)
	{
		int32 BytesReceived = 0;
		// XXX: 자주 오지 않는 데이터가 추가될 경우 HasPendingData()를 고려.
		if (sock_->Recv(ReceivedData.GetData(), BufferSize, BytesReceived))
		{
			if (BytesReceived > 0)
			{
				Message msg = Marshaller::DeserializeMessage(ReceivedData);
				MessageHandler.HandleMessage(msg);
				ReceivedData.SetNumZeroed(BufferSize);
			}
		}
		FPlatformProcess::Sleep(0.1);
	}
	return 0;
}

void FReceiveTask::Stop()
{
	bShouldRun_ = false;
}
