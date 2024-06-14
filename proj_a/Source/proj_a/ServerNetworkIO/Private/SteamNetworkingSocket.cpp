#include "SteamNetworkingSocket.h"

#include "FNetLogger.h"
#include "isteamnetworkingsockets.h"
#include "Marshaller.h"

SteamNetworkingSocket::SteamNetworkingSocket()
{
	poll_group_ = SteamNetworkingSockets()->CreatePollGroup();
	if (poll_group_ == k_HSteamNetPollGroup_Invalid)
	{
		throw std::runtime_error("Failed to create poll group.");
	}
	// steam_listen_socket_ = SteamNetworkingSockets()->CreateListenSocketP2P(n_local_virtual_port, 0, &opt);
	local_address_.Clear();
	local_address_.m_port = 4242;
	steam_listen_socket_ = SteamNetworkingSockets()->CreateListenSocketIP(local_address_, 0, nullptr);
	if (steam_listen_socket_ == k_HSteamListenSocket_Invalid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create listen socket."));
		throw std::runtime_error("Failed to create listen socket.");
	}
	UE_LOG(LogTemp, Warning, TEXT("Listen Socket Created"));
	this_thread_ = FRunnableThread::Create(this, TEXT("SteamNetworkingSocket"));
}

SteamNetworkingSocket::~SteamNetworkingSocket()
{
	SteamNetworkingSockets()->DestroyPollGroup(poll_group_);
	SteamNetworkingSockets()->CloseListenSocket(steam_listen_socket_);
}

uint32 SteamNetworkingSocket::Run()
{
	while (true)
	{
		SteamAPI_RunCallbacks();
		this->RecieveMessages();
		this->FlushSendMessages();
	}
}

void SteamNetworkingSocket::RecieveMessages()
{
	const int n_max_message = 10;
	SteamNetworkingMessage_t* pp_message[n_max_message];
	int n_messages = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(poll_group_, pp_message, n_max_message);

	for (int i = 0; i < n_messages; ++i)
	{
		// TODO: Handle message
		char* msg = static_cast<char*>(const_cast<void*>(pp_message[i]->GetData()));
		const int size = pp_message[i]->GetSize();
		
		TArray<uint8> data(reinterpret_cast<uint8*>(msg), size);
		Message ret_msg = Marshaller::DeserializeMessage(data);
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Recieved Message: %s"), *FString(ret_msg.DebugString().c_str()));
		pp_message[i]->Release();
		recieve_buffer_.Push(ret_msg);
		data.SetNumZeroed(data.Num());
	}
}

void SteamNetworkingSocket::FlushSendMessages()
{
	send_buffer_.Swap();
	std::queue<Message> send_buffer = send_buffer_.GetReadBuffer();
	while (!send_buffer.empty())
	{
		Message msg = send_buffer.front();
		TArray<uint8> data = Marshaller::SerializeMessage(msg);
		for (auto& conn : steam_connections_)
		{
			SteamNetworkingSockets()->SendMessageToConnection(conn, data.GetData(), data.Num(), k_nSteamNetworkingSend_Unreliable, nullptr);
		}
		send_buffer.pop();
		data.SetNumZeroed(data.Num());
	}
}

std::queue<Message> SteamNetworkingSocket::GetReadBuffer()
{
	recieve_buffer_.Swap();
	return recieve_buffer_.GetReadBuffer();
}

void SteamNetworkingSocket::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info)
{
	switch (info->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_Connecting:
		if (SteamNetworkingSockets()->AcceptConnection(info->m_hConn) == k_EResultOK)
		{
			UE_LOG(LogTemp, Warning, TEXT("Client Connecting"));
		}
		else
		{
			SteamNetworkingSockets()->CloseConnection(info->m_hConn, 0, "Failed to accept connection", true);
			UE_LOG(LogTemp, Error, TEXT("Failed to accept connection."));
		}
		break ;
	case k_ESteamNetworkingConnectionState_Connected:
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Client Connected"));
		UE_LOG(LogTemp, Warning, TEXT("Client Connected"));
		steam_connections_.push_back(info->m_hConn);
		num_clients_ += 1;
		SteamNetworkingSockets()->SetConnectionPollGroup(info->m_hConn, this->poll_group_);
		if (num_clients_ == MAX_CLIENTS)
		{
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Game Started"));
			UE_LOG(LogTemp, Warning, TEXT("Game Started"));
			b_is_game_stated = true;
		}
		break ;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Closed By Peer"));
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Client Disconnected"));
		SteamNetworkingSockets()->CloseConnection(info->m_hConn, 0, nullptr, false);
		break ;
	default:
		break;
	}
}

bool SteamNetworkingSocket::IsGameStarted() const
{
	return b_is_game_stated;
}

