#include "ClientNetworkManager.h"

#include "FDataHub.h"
#include "ISocketFactory.h"
#include "MessageHandler.h"

void UClientNetworkManager::Initialize(ZOOMIES::ESocketType SocketType)
{
	ISocketInterface* SocketInterface = ISocketFactory::CreateSocketInterface(SocketType);
	check(SocketInterface);
	SocketInterface->SetAsClient();
	SocketInterface->ActivateClient();

	Worker = NewObject<UNetworkWorker>();
	Worker->Initialize(SocketInterface);
	Worker->SetMessageReceivedCallback([this](const Message& Data)
	{
		this->OnDataReceived(Data);
	});
}

void UClientNetworkManager::OnDataReceived(const Message& Data)
{
	FMessageHandler MessageHandler;

	MessageHandler.HandleMessage(Data);
}

void UClientNetworkManager::SendData(const Message& Data)
{
	Worker->SendData(Data);
}

void UClientNetworkManager::Shutdown()
{
	Worker->Stop();
}

UClientNetworkManager::~UClientNetworkManager()
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

