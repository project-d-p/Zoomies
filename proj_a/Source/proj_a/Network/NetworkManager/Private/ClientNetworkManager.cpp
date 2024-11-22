#include "ClientNetworkManager.h"

#include "FDataHub.h"
#include "FNetLogger.h"
#include "ISocketFactory.h"
#include "MessageHandler.h"

UClientNetworkManager::UClientNetworkManager()
{
}

void UClientNetworkManager::Initialize(ENetworkTypeZoomies SocketType)
{
	Worker = NewObject<UNetworkWorker>(this);
	check(Worker);

	UISocketInterface* SocketInterface = SocketFactory->CreateSocketInterface(SocketType, Worker);
	check(SocketInterface);
	SocketInterface->ActivateClient();
	
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
	{
		Worker->Stop();
		// WorkerThread�� �����ϰ� ����� ������ ���
		if (WorkerThread)
		{
			WorkerThread->WaitForCompletion();
			delete WorkerThread; // ���� �޸� �Ҵ� ����
			WorkerThread = nullptr;
		}

		Worker = nullptr; // Worker ������ ����
	}
}

UClientNetworkManager::~UClientNetworkManager()
{
	// UClientNetworkManager::Shutdown();
	if (WorkerThread)
	{
		WorkerThread = nullptr;
	}
}

