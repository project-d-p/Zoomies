#include "NetworkWorker.h"

#include "ISocketInterface.h"
#include "message.pb.h"
#include "steam_api_common.h"

UNetworkWorker::UNetworkWorker()
{
}

void UNetworkWorker::Initialize(UISocketInterface* socketInterface)
{
	this->SocketInterface = socketInterface;
}

void UNetworkWorker::SetMessageReceivedCallback(TFunction<void(const Message&)> Callback)
{
	this->DataRecieveCallback = Callback;
}

void UNetworkWorker::SendData(const Message& Data)
{
	SendBuffer.Push(Data);
}

void UNetworkWorker::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function)
{
	SocketInterface->SetGameStartCallback(NumOfPlayers, Function);
}

uint32 UNetworkWorker::Run()
{
	while (this->bIsRunning)
	{
		SteamAPI_RunCallbacks();
		SocketInterface->RecieveData(DataRecieveCallback);
		this->FlushSendMessages();
		FPlatformProcess::Sleep(0.01);
	}
	return 0;
}

void UNetworkWorker::Stop()
{
	FRunnable::Stop();

	this->bIsRunning = false;
}

UNetworkWorker::~UNetworkWorker()
{
	if (SocketInterface->IsValidLowLevel())
	{
		SocketInterface->ConditionalBeginDestroy();
		SocketInterface = nullptr;
	}
}

void UNetworkWorker::FlushSendMessages()
{
	SendBuffer.Swap();
	std::queue<Message> send_buffer = SendBuffer.GetReadBuffer();

	while (!send_buffer.empty())
	{
		Message& msg = send_buffer.front();
		SocketInterface->SendData(msg);
		send_buffer.pop();
	}
}
