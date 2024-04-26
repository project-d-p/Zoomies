#include "FTcpSendTask.h"

#include "SocketManager.h"

FTcpSendTask::FTcpSendTask()
{
	USocketManager* SocketManager = USocketManager::getInstance();
	sock_ = SocketManager->getTCPSocket();
	thread_ = FRunnableThread::Create(this, TEXT("NetworkTaskThread"), 0, TPri_BelowNormal);
}

uint32 FTcpSendTask::Run()
{
	// FInputData InputData;
	//
	// while (bShouldRun_)
	// {
	// 	FVector2D TotalData = FVector2D(0, 0);
	// 	while (InputQueue.Dequeue(InputData))
	// 	{
	// 		if (InputData.Type == 0)
	// 		{
	// 			TotalData += InputData.InputVector;
	// 		}
	// 	}
	// 	Vec3 *InputProgressData = ProtobufUtility::ConvertToFVecToVec3(FVector(TotalData, 0));
	// 	ProtoData.mutable_progess_vector()->CopyFrom(*InputProgressData);
	// 	ProtoData.set_player_id("1");
	// 	ProtoData.set_state(State::STATE_RUN);
	// 	ProtoData.set_timestamp("1");
	// 	
	// 	Message msg;
	// 	*msg.mutable_movement() = ProtoData;
	// 	
	// 	TArray<uint8> bData = Marshaller::SerializeMessage(msg);
	//
	// 	int32 BytesSent = 0;
	// 	sock_->Send(bData.GetData(), bData.Num(), BytesSent);
	// 	delete(InputProgressData);
	// 	
	// 	FPlatformProcess::Sleep(0.05);
	// }
	return 0;
}
