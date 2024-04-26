#include "FReceiveTask.h"
#include "SocketManager.h"
#include "Marshaller.h"
#include "Sockets.h"
#include "message.pb.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

FReceiveTask::FReceiveTask(bool bUseTCP, int32 InPort)
{
	USocketManager* SocketManager = USocketManager::getInstance();
	sock_ = bUseTCP ? SocketManager->getTCPSocket() : SocketManager->getUDPSocket();
	if (!bUseTCP) {
		EnsureUdpSocketBound(InPort);
	}
	bShouldRun_ = true;
	thread_ = FRunnableThread::Create(this, TEXT("NetworkReceiverThread"), 0, TPri_BelowNormal);
}

void FReceiveTask::EnsureUdpSocketBound(int32 InPort)
{
	if (sock_ && sock_->GetSocketType() == SOCKTYPE_Datagram)
	{
		FIPv4Endpoint Endpoint(FIPv4Address::Any, InPort);
		int32 BufferSize = 2 * 1024 * 1024;
		sock_->SetReceiveBufferSize(BufferSize, BufferSize);
		bool bIsBound = sock_->Bind(*Endpoint.ToInternetAddr());
		check(bIsBound);
	}
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
		// FPlatformProcess::Sleep(0.01);
	}
	return 0;
}

void FReceiveTask::Stop()
{
	bShouldRun_ = false;
}
