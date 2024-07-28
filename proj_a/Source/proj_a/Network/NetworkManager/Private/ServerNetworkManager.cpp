#include "ServerNetworkManager.h"

#include "ISocketFactory.h"
#include "ISocketInterface.h"
#include "NetworkWorker.h"

void UServerNetworkManager::Initialize(ZOOMIES::ESocketType SocketType)
{
	ISocketInterface* SocketInterface = ISocketFactory::CreateSocketInterface(SocketType);
	check(SocketInterface);
	SocketInterface->SetAsServer();
	SocketInterface->ActivateServer();

	Worker = NewObject<UNetworkWorker>();
	Worker->Initialize(SocketInterface);
	Worker->SetMessageReceivedCallback([this](const Message& Data)
	{
		this->OnDataReceived(Data);		
	});
}

void UServerNetworkManager::OnDataReceived(const Message& Data)
{
	RecievedMessageBuffer.Push(Data);
}

void UServerNetworkManager::SendData(const Message& Data)
{
	Worker->SendData(Data);
}

void UServerNetworkManager::Shutdown()
{
	Worker->Stop();
}

void UServerNetworkManager::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Callback)
{
	Worker->SetGameStartCallback(NumOfPlayers, Callback);
}

UServerNetworkManager::~UServerNetworkManager()
{
	if (WorkerThread)
	{
		WorkerThread->Kill();
		WorkerThread->WaitForCompletion();
		delete WorkerThread;
		WorkerThread = nullptr;
	}
	if (Worker)
	{
		Worker->ConditionalBeginDestroy();
		Worker = nullptr;
	}
}
