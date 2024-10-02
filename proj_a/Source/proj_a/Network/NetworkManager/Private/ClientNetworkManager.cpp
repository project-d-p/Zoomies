#include "ClientNetworkManager.h"

#include "FDataHub.h"
#include "FNetLogger.h"
#include "ISocketFactory.h"
#include "MessageHandler.h"

void UClientNetworkManager::Initialize(ENetworkTypeZoomies SocketType)
{
	UISocketInterface* SocketInterface = SocketFactory->CreateSocketInterface(SocketType);
	check(SocketInterface);
	SocketInterface->ActivateClient();

	Worker = NewObject<UNetworkWorker>();
	check(Worker);
	Worker->Initialize(SocketInterface);
	Worker->SetMessageReceivedCallback([this](const Message& Data)
	{
		// FNetLogger::EditerLog(FColor::Blue, TEXT("%s"), *FString(Data.DebugString().c_str()));
		this->OnDataReceived(Data);
	});
	WorkerThread = FRunnableThread::Create(Worker, TEXT("NetworkWorker"));
}

void UClientNetworkManager::OnDataReceived(const Message& Data)
{
	MessageHandler.HandleMessage(Data);
}

void UClientNetworkManager::SendData(const Message& Data)
{
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

