#include "FReceiveTask.h"
#include "SocketManager.h"
#include "Marshaller.h"
#include "Sockets.h"
#include "message.pb.h"

DEFINE_LOG_CATEGORY(LogNetwork);

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

				Movement pos;
				// FString TestStringLog;
				// int CountNonZero = 0;
				switch (msg.message_type_case())
				{
				case Message::kMovement:
					// UE_LOG(LogNetwork, Log, TEXT("Movement message received"));
					// pos = msg.movement();
					// UE_LOG(LogNetwork, Log, TEXT("Player ID: %s"), *FString(pos.player_id().c_str()));
					// UE_LOG(LogNetwork, Log, TEXT("Position: (%f, %f, %f)"), pos.progess_vector().x(),
					//        pos.progess_vector().y(), pos.progess_vector().z());
					// UE_LOG(LogNetwork, Log, TEXT("Orientation: (%f, %f, %f)"), pos.orientation().x(),
					//        pos.orientation().y(), pos.orientation().z());
					// UE_LOG(LogNetwork, Log, TEXT("State: %d"), static_cast<int>(pos.state()));
					// UE_LOG(LogNetwork, Log, TEXT("Timestamp: %s"), *FString(pos.timestamp().c_str()));
						
					// for (auto Data : ReceivedData)
					// {
					// 	TestStringLog.AppendInt(Data);
					// }
					// UE_LOG(LogNetwork, Log, TEXT("ReceivedData: %s"), *TestStringLog);
					// CountNonZero = 0;
					// for (uint8 Value : ReceivedData) {
					// 	if (Value != 0) {
					// 		++CountNonZero;
					// 	}
					// }
					// UE_LOG(LogNetwork, Log, TEXT("DataSize: %d"), CountNonZero);
					UE_LOG(LogNetwork, Log, TEXT("%s"), *FString(msg.DebugString().c_str()));
					break;
				default:
					// UE_LOG(LogNetwork, Log, TEXT("%s"), *FString(msg.DebugString().c_str()));
					UE_LOG(LogNetwork, Log, TEXT("Unknown message type"));
					break;
				}
			}
			ReceivedData.SetNumZeroed(BufferSize);
		}
		FPlatformProcess::Sleep(0.01);
	}
	return 0;
}

void FReceiveTask::Stop()
{
	bShouldRun_ = false;
}
