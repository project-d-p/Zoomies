#include "SteamSocket.h"

#include "FNetLogger.h"
#include "Marshaller.h"
#include "isteamnetworkingsockets.h"
#include "isteamnetworkingutils.h"

USteamSocket::USteamSocket(): MaxClients(0), bIsServer(false)
{
}

void USteamSocket::RecieveData(const TFunction<void(const Message&)>& Callback)
{
	const int n_max_message = 10;
	SteamNetworkingMessage_t* pp_message[n_max_message];
	int n_messages = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(PollGroup, pp_message, n_max_message);
	if (n_messages == 0)
	{
		return ;
	}

	
	for (int i = 0; i < n_messages; ++i)
	{
		if (pp_message[i] == nullptr)
			continue;

		const uint8* msg = static_cast<const uint8*>(pp_message[i]->GetData());
		int32 size = pp_message[i]->m_cbSize;

		TArray<uint8> data;
		data.SetNum(size);
		FMemory::Memcpy(data.GetData(), msg, size);
		
		Message ret_msg = Marshaller::DeserializeMessage(data);
		Callback(ret_msg);
		pp_message[i]->Release();
	}
}

void USteamSocket::SendData(Message& Msg)
{
	if (Connections.Num() == 0)
	{
		return ;
	}
	
	TArray<uint8> data = Marshaller::SerializeMessage(Msg);

	std::vector<SteamNetworkingMessage_t*> messages;
	messages.reserve(Connections.Num());
	
	for (auto& conn : Connections)
	{
		SteamNetworkingMessage_t* pMsg = SteamNetworkingUtils()->AllocateMessage(data.Num());
		if (pMsg)
		{
			FMemory::Memcpy(pMsg->m_pData, data.GetData(), data.Num());
			pMsg->m_cbSize = data.Num();
			pMsg->m_nFlags = k_nSteamNetworkingSend_Unreliable;
			pMsg->m_conn = conn;
			messages.push_back(pMsg);
		}
	}

	if (!messages.empty())
	{
		SteamNetworkingSockets()->SendMessages(messages.size(), messages.data(), nullptr);
		messages.clear();
	}
}

// void USteamSocket::SendData(Message& Msg)
// {
// 	if (Connections.Num() == 0)
// 	{
// 		return ;
// 	}
// 	
// 	TArray<uint8> data = Marshaller::SerializeMessage(Msg);
// 	
// 	for (auto& conn : Connections)
// 	{
// 		SteamNetworkingSockets()->SendMessageToConnection(conn, data.GetData(), data.Num(), k_nSteamNetworkingSend_Unreliable, nullptr);
// 	}
// }

void USteamSocket::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function)
{
	MaxClients = NumOfPlayers;
	GameStartCallback = Function;

	/* Test */
	if (NumOfPlayers == 1)
	{
		GameStartCallback();
	}
}

void USteamSocket::SetAsServer()
{
	bIsServer = true;
}

void USteamSocket::SetAsClient()
{
	bIsServer = false;
}

USteamSocket::~USteamSocket()
{
	for (auto& conn : Connections)
	{
		SteamNetworkingSockets()->CloseConnection(conn, 0, nullptr, false);
	}
	if (PollGroup != k_HSteamNetPollGroup_Invalid)
	{
		SteamNetworkingSockets()->DestroyPollGroup(PollGroup);
	}
}

void USteamSocket::AddConnection(HSteamNetConnection Connection)
{
	Connections.Add(Connection);
	SteamNetworkingSockets()->SetConnectionPollGroup(Connection, PollGroup);
}

void USteamSocket::CloseConnection(HSteamNetConnection Connection)
{
	Connections.Remove(Connection);
	SteamNetworkingSockets()->CloseConnection(Connection, 0, nullptr, false);
}

void USteamSocket::BeginDestroy()
{
	Super::BeginDestroy();
}

void USteamSocket::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pParam)
{
	if (bIsServer)
	{
		OnServerCallBack(pParam);
	}
	else
	{
		OnClientCallBack(pParam);
	}
}

// void USteamSocket::OnNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pParam)
// {
// 	if (bIsServer)
// 	{
// 		OnServerCallBack(pParam);
// 	}
// 	else
// 	{
// 		OnClientCallBack(pParam);
// 	}
// }

void USteamSocket::OnServerCallBack(SteamNetConnectionStatusChangedCallback_t* pParam)
{
	switch (pParam->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_Connecting:
		if (SteamNetworkingSockets()->AcceptConnection(pParam->m_hConn) == k_EResultOK)
		{
			FNetLogger::LogError(TEXT("Connection Accepted"));
		}
		else
		{
			FNetLogger::LogError(TEXT("Connection Rejected"));
		}
		break;
	case k_ESteamNetworkingConnectionState_Connected:
		this->AddConnection(pParam->m_hConn);
		if (Connections.Num() == MaxClients - 1)
			this->GameStartCallback();
		break ;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
		FNetLogger::LogError(TEXT("Connection Closed By Peer:  %d, pParam: %hs"), pParam->m_info.m_eEndReason, UTF8_TO_TCHAR(pParam->m_info.m_szEndDebug));
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		UE_LOG(LogTemp, Warning, TEXT("Connection Closed"));
		this->CloseConnection(pParam->m_hConn);
		break ;
	default:
		break;
	}
}

void USteamSocket::OnClientCallBack(SteamNetConnectionStatusChangedCallback_t* pParam)
{
	switch (pParam->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_Connecting:
		break;
	case k_ESteamNetworkingConnectionState_Connected:
		break ;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
		FNetLogger::LogError(TEXT("[Client]Connection Closed By Peer:  %d, pParam: %hs"), pParam->m_info.m_eEndReason, UTF8_TO_TCHAR(pParam->m_info.m_szEndDebug));
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		UE_LOG(LogTemp, Warning, TEXT("[Client]Connection Closed"));
		this->CloseConnection(pParam->m_hConn);
		break ;
	default:
		break;
	}
}

UISocketInterface* USteamSocket::Clone() const
{
	return nullptr;
}

void USteamSocket::ActivateServer()
{
	Super::ActivateServer();

	SetAsServer();
	
	PollGroup = SteamNetworkingSockets()->CreatePollGroup();
	if (PollGroup == k_HSteamNetPollGroup_Invalid)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create poll group"));
		check(false)
	}
}

void USteamSocket::ActivateClient()
{
	Super::ActivateClient();

	SetAsClient();
	
	PollGroup = SteamNetworkingSockets()->CreatePollGroup();
	if (PollGroup == k_HSteamNetPollGroup_Invalid)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create poll group"));
		check(false)
	}
}
