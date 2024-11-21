#include "ServerNetworkManager.h"

#include "FNetLogger.h"
#include "ISocketFactory.h"
#include "ISocketInterface.h"
#include "NetworkWorker.h"

void UServerNetworkManager::Initialize(ENetworkTypeZoomies SocketType)
{
	UISocketInterface* SocketInterface = SocketFactory->CreateSocketInterface(SocketType, this);
	check(SocketInterface);
	SocketInterface->ActivateServer();

	Worker = NewObject<UNetworkWorker>(this);
	check(Worker);
	Worker->Initialize(SocketInterface);
	Worker->SetMessageReceivedCallback([this](const Message& Data)
	{
		this->OnDataReceived(Data);		
	});
	WorkerThread = FRunnableThread::Create(Worker, TEXT("NetworkWorker"));
}

void UServerNetworkManager::OnDataReceived(const Message& Data)
{
	RecievedMessageBuffer.Push(Data);
}

void UServerNetworkManager::SendData(const Message& Data)
{
	if (Worker)
		Worker->SendData(Data);
}

void UServerNetworkManager::Shutdown()
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

void UServerNetworkManager::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Callback)
{
	if (Worker)	
		Worker->SetGameStartCallback(NumOfPlayers, Callback);
}

UServerNetworkManager::~UServerNetworkManager()
{
	// UServerNetworkManager::Shutdown();
	if (WorkerThread)
	{
		WorkerThread = nullptr;
	}
}
