#include "FUdpReceiveTask.h"

#include "SocketManager.h"
#include "Marshaller.h"
#include "MessageHandler.h"
#include "FNetLogger.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

FUdpReceiveTask::FUdpReceiveTask(const FString Ip, int32 Port)
{
	if (InitializeSocket(Ip, Port))
	{
		Start(TEXT("NetworkReceiverThreadUDP"));
	}
	else
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to initialize UDP socket"));
	}
}

FUdpReceiveTask::~FUdpReceiveTask()
{
	UE_LOG(LogTemp, Warning, TEXT("UdpReceiveTask is destroyed."));
	if (Thread)
	{
		delete Thread;
		Thread = nullptr;
	}
}


bool FUdpReceiveTask::InitializeSocket(const FString Ip, int32 Port)
{
	FSocketManager &sockManager = FSocketManager::GetInstance();
	Socket = sockManager.GetUDPSocket();
	if (!Socket && Socket->GetSocketType() == SOCKTYPE_Datagram)
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to get UDP socket"));
		return false;
	}
	FIPv4Address parseIp;
	FIPv4Address::Parse(Ip, parseIp);
	FIPv4Endpoint endPoint(parseIp, Port);

	int32 desireBufferSize = 1024;
	int32 actualSize = 0;
	if (!Socket->SetReceiveBufferSize(desireBufferSize, actualSize))
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to set the receive buffer size."));
	}
	if (desireBufferSize != actualSize)
	{
		FNetLogger::GetInstance().LogInfo(TEXT("Attempted to set buffer size to %d bytes."), desireBufferSize);
		FNetLogger::GetInstance().LogInfo(TEXT("Actual buffer size set: %d bytes."), actualSize);
	}
	if (!Socket->Bind(*endPoint.ToInternetAddr()))
	{
		FNetLogger::GetInstance().LogError(TEXT("Failed to bind UDP socket"));
	}
	return true;
}


uint32 FUdpReceiveTask::Run()
{
	const int32 bufferSize = 1024;
	TArray<uint8> receiveData;
	FMessageHandler MessageHandler;

	receiveData.Reserve(bufferSize);
	receiveData.SetNumZeroed(bufferSize);
	while (ShouldRun)
	{
		//FNetLogger::GetInstance().LogInfo(TEXT("UdpReceiveTask is running."));
		int32 bytesReceived = 0;
		if (Socket->Recv(receiveData.GetData(), bufferSize, bytesReceived))
		{
			if (bytesReceived > 0)
			{
				Message msg = Marshaller::DeserializeMessage(receiveData);
				MessageHandler.HandleMessage(msg);
				receiveData.SetNumZeroed(bufferSize);
			}
		}
		FPlatformProcess::Sleep(0.01);
	}
	return 0;
}
