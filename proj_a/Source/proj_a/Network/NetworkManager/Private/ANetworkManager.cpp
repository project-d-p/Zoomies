#include "ANetworkManager.h"
#include "ISocketFactory.h"
#include "ISocketInterface.h"

void UANetworkManager::Initialize(ZOOMIES::ESocketType SocketType)
{
}

void UANetworkManager::OnDataReceived(const Message& Data)
{
}

void UANetworkManager::SendData(const Message& Data)
{
}

void UANetworkManager::Shutdown()
{
}

void UANetworkManager::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Callback)
{
}

UANetworkManager::~UANetworkManager()
{
}

std::queue<Message> UANetworkManager::GetRecievedMessages()
{
	RecievedMessageBuffer.Swap();
	return RecievedMessageBuffer.GetReadBuffer();
}
