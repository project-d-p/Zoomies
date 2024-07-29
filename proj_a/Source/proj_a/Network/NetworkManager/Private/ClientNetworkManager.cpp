#include "ClientNetworkManager.h"

#include "FDataHub.h"
#include "FNetLogger.h"
#include "ISocketFactory.h"
#include "MessageHandler.h"

void UClientNetworkManager::Initialize(ENetworkTypeZoomies SocketType)
{
	UISocketInterface* SocketInterface = SocketFactory->CreateSocketInterface(SocketType);
	check(SocketInterface);
	SocketInterface->SetAsClient();
	SocketInterface->ActivateClient();

	Worker = NewObject<UNetworkWorker>();
	check(Worker);
	Worker->Initialize(SocketInterface);
	Worker->SetMessageReceivedCallback([this](const Message& Data)
	{
		this->OnDataReceived(Data);
	});
	WorkerThread = FRunnableThread::Create(Worker, TEXT("NetworkWorker"));
}

void UClientNetworkManager::OnDataReceived(const Message& Data)
{
	FMessageHandler MessageHandler;

	MessageHandler.HandleMessage(Data);
}

void UClientNetworkManager::SendData(const Message& Data)
{
	FNetLogger::LogError(TEXT("SENDING DATA"));
	if (Worker)
		Worker->SendData(Data);
}

void UClientNetworkManager::Shutdown()
{
	if (Worker)
		Worker->Stop();
}

UClientNetworkManager::~UClientNetworkManager()
{
	UClientNetworkManager::Shutdown();
	if (WorkerThread)
	{
		WorkerThread->Kill();
		WorkerThread->WaitForCompletion();
		delete WorkerThread;
		WorkerThread = nullptr;
	}
}

