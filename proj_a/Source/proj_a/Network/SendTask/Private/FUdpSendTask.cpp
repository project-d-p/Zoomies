#include "FUdpSendTask.h"

#include "SocketManager.h"
#include "Sockets.h"
#include "message.pb.h"
#include "ProtobufUtility.h"
#include "Common/UdpSocketBuilder.h"
#include "Marshaller.h"
#include "FNetLogger.h"

Movement FBaseSendTask::ProtoData;
TQueue<FInputData, EQueueMode::Spsc> FBaseSendTask::InputQueue;

FUdpSendTask::FUdpSendTask()
{
	USocketManager* SocketManager = USocketManager::GetInstance();
	Sock = SocketManager->GetUDPSocket();
	if (Sock == nullptr)
	{
		FNetLogger::GetInstance().LogError(TEXT("UDP Socket is null!"));
		return;
	}
	Thread = FRunnableThread::Create(this, TEXT("NetworkTaskThread"), 0, TPri_BelowNormal);
	if (!Thread)
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to create UDP thread"));
	}
}

uint32 FUdpSendTask::Run()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	USocketManager* SocketManager = USocketManager::GetInstance();

	SocketManager->GetTCPSocket()->GetPeerAddress(*Addr);
	uint32 uintIp;
	Addr->GetIp(uintIp);
	
	FIPv4Address IP(uintIp);
	// TODO: 주소 문제 해결
	// FIPv4Address IP2(10, 19, 225, 124);
	int32 Port = 5000;
	FIPv4Endpoint Endpoint(IP, Port);
		
	FString AddressString = Endpoint.ToText().ToString();
	bool bIsValid;
	Addr->SetIp(*AddressString, bIsValid);
	if (!bIsValid)
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to set IP"));
		return 1;
	}
	Addr->SetPort(Port);
	
	while (bShouldRun)
	{
		FInputData InputData;
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
		if (Sock->SendTo(bData.GetData(), bData.Num(), BytesSent, *Addr))
		{
			FNetLogger::GetInstance().LogInfo(TEXT("Sent %d bytes to %s"), BytesSent, *AddressString);
		}
		else
		{
			FNetLogger::GetInstance().LogError(TEXT("Failed to send data"));
		}

		FPlatformProcess::Sleep(0.1);
	}
	return 0;
}
