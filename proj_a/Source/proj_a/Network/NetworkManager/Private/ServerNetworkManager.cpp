#include "ServerNetworkManager.h"

#include "FNetLogger.h"
#include "ISocketFactory.h"
#include "ISocketInterface.h"
#include "NetworkWorker.h"

void UServerNetworkManager::Initialize(ENetworkTypeZoomies SocketType)
{
	if (GetWorld())
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("UServerNetworkManager::Initialize() GetWorld()"));
	}
	else
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Failed UServerNetworkManager::Initialize() GetWorld()"));
	}
	UISocketInterface* SocketInterface = SocketFactory->CreateSocketInterface(SocketType);
	check(SocketInterface);
	SocketInterface->ActivateServer();

	Worker = NewObject<UNetworkWorker>();
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
		Worker->Stop();
}

void UServerNetworkManager::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Callback)
{
	if (Worker)	
		Worker->SetGameStartCallback(NumOfPlayers, Callback);
}

UServerNetworkManager::~UServerNetworkManager()
{
	UServerNetworkManager::Shutdown();
	if (WorkerThread)
	{
		WorkerThread->Kill();
		WorkerThread->WaitForCompletion();
		delete WorkerThread;
		WorkerThread = nullptr;
	}
}
