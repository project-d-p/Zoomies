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
		// WorkerThread가 안전하게 종료될 때까지 대기
		if (WorkerThread)
		{
			WorkerThread->WaitForCompletion();
			delete WorkerThread; // 동적 메모리 할당 해제
			WorkerThread = nullptr;
		}

		Worker = nullptr; // Worker 포인터 정리
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

