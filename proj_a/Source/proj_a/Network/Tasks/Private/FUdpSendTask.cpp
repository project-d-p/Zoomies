#include "FUdpSendTask.h"

#include "SocketManager.h"
#include "Marshaller.h"
#include "Sockets.h"
#include "message.pb.h"
#include "ProtobufUtility.h"
#include "Common/UdpSocketBuilder.h"

Movement FBaseSendTask::ProtoData;
TQueue<FInputData, EQueueMode::Spsc> FBaseSendTask::InputQueue;

FUdpSendTask::FUdpSendTask()
{
	USocketManager* SocketManager = USocketManager::getInstance();
	sock_ = SocketManager->getUDPSocket();
	thread_ = FRunnableThread::Create(this, TEXT("NetworkTaskThread"), 0, TPri_BelowNormal);
}

uint32 FUdpSendTask::Run()
{
	FIPv4Address IP;
	FIPv4Address::Parse("10.19.225.124", IP);
	int32 Port = 5000;
	FIPv4Endpoint Endpoint(IP, Port);
		
	FString AddressString = Endpoint.ToText().ToString();
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	bool bIsValid;
	Addr->SetIp(*AddressString, bIsValid);
	Addr->SetPort(Port);
	
	FInputData InputData;
	while (bShouldRun_)
	{
		FVector2D TotalData = FVector2D(0, 0);
		while (InputQueue.Dequeue(InputData))
		{
			if (InputData.Type == 0)
			{
				TotalData += InputData.InputVector;
			}
		}

		Vec3* InputProgressData = ProtobufUtility::ConvertToFVecToVec3(FVector(TotalData, 0));
		ProtoData.set_allocated_progess_vector(InputProgressData);
		ProtoData.set_player_id("1");
		ProtoData.set_state(State::STATE_RUN);
		ProtoData.set_timestamp("1");

		Message msg;
		*msg.mutable_movement() = ProtoData;

		TArray<uint8> bData = Marshaller::SerializeMessage(msg);

		int32 BytesSent = 0;
		if (sock_->SendTo(bData.GetData(), bData.Num(), BytesSent, *Addr))
		{
			UE_LOG(LogTemp, Log, TEXT("Sent %d bytes to %s"), BytesSent, *AddressString);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send data"));
		}

		FPlatformProcess::Sleep(0.1);
	}
	return 0;
}
