// #include "FBaseSendTask.h"
// #include "SocketManager.h"
// #include "Marshaller.h"
// #include "ProtobufUtility.h"
// #include "Sockets.h"
// #include "message.pb.h"
//
// Movement FBaseSendTask::ProtoData;
// TQueue<FInputData, EQueueMode::Spsc> FBaseSendTask::InputQueue;
//
// FBaseSendTask::FBaseSendTask(bool bUseTCP)
// {
// 	USocketManager* SocketManager = USocketManager::getInstance();
// 	sock_ = bUseTCP ? SocketManager->getTCPSocket() : SocketManager->getUDPSocket();
// 	bShouldRun_ = true;
// 	thread_ = FRunnableThread::Create(this, TEXT("NetworkTaskThread"), 0, TPri_BelowNormal);
// }
//
// FBaseSendTask::~FBaseSendTask()
// {
// 	if (thread_)
// 	{
// 		thread_->Kill(true);
// 		delete thread_;
// 	}
// }
//
// uint32 FBaseSendTask::Run()
// {
// 	FInputData InputData;
// 	
// 	while (bShouldRun_)
// 	{
// 		FVector2D TotalData = FVector2D(0, 0);
// 		while (InputQueue.Dequeue(InputData))
// 		{
// 			if (InputData.Type == 0)
// 			{
// 				TotalData += InputData.InputVector;
// 			}
// 		}
// 		Vec3* InputProgressData = ProtobufUtility::ConvertToFVecToVec3(FVector(TotalData, 0));
// 		ProtoData.set_allocated_progess_vector(InputProgressData);
// 		ProtoData.set_player_id("1");
// 		ProtoData.set_state(State::STATE_RUN);
// 		ProtoData.set_timestamp("1");
// 		
// 		Message msg;
// 		*msg.mutable_movement() = ProtoData;
// 		
// 		TArray<uint8> bData = Marshaller::SerializeMessage(msg);
//
// 		int32 BytesSent = 0;
// 		sock_->Send(bData.GetData(), bData.Num(), BytesSent);
// 		
// 		FPlatformProcess::Sleep(0.1);
// 	}
// 	return 0;
// }
//
// void FBaseSendTask::Stop()
// {
// 	bShouldRun_ = false;
// }
