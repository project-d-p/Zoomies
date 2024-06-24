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

	opt_.m_eValue = k_ESteamNetworkingConfig_IP_AllowWithoutAuth;
	opt_.m_eDataType = k_ESteamNetworkingConfig_Int32;
	opt_.m_val.m_int32 = 1;
	
	steam_listen_socket_ = SteamNetworkingSockets()->CreateListenSocketIP(local_address_, 1, &opt_);
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
	this->SteamNetworkingSocket::Stop();
	if (this_thread_ != nullptr)
	{
		this_thread_->Kill(true);
		this_thread_->WaitForCompletion();
		delete this_thread_;
		this_thread_ = nullptr;
	}
	for (auto& conn : steam_connections_)
	{
		SteamNetworkingSockets()->CloseConnection(conn, 0, nullptr, false);
	}
	if (steam_listen_socket_ != k_HSteamListenSocket_Invalid)
	{
		SteamNetworkingSockets()->CloseListenSocket(steam_listen_socket_);
	}
	if (poll_group_ != k_HSteamNetPollGroup_Invalid)
		SteamNetworkingSockets()->DestroyPollGroup(poll_group_);
}

uint32 SteamNetworkingSocket::Run()
{
	while (!bStop)
	{
		SteamAPI_RunCallbacks();
		this->RecieveMessages();
		this->FlushSendMessages();
		FPlatformProcess::Sleep(0.01);
	}
	return 0;
}

void SteamNetworkingSocket::RecieveMessages()
{
	const int n_max_message = 10;
	SteamNetworkingMessage_t* pp_message[n_max_message];
	int n_messages = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(poll_group_, pp_message, n_max_message);
	if (n_messages == 0)
	{
		return ;
	}
	TArray<uint8> data;
	data.Reserve(1024);
	
	for (int i = 0; i < n_messages; ++i)
	{
		if (pp_message[i] == nullptr)
			continue;
		data.Reset();
		const uint8* msg = static_cast<const uint8*>(pp_message[i]->GetData());
		int32 size = pp_message[i]->m_cbSize;
		data.Append(msg, size);
		try {
			Message ret_msg = Marshaller::DeserializeMessage(data);
			pp_message[i]->Release();
			recieve_buffer_.Push(ret_msg);
		} catch (const std::exception& e) {
			FNetLogger::EditerLog(FColor::Red, TEXT("Failed to deserialize message: %s"), *FString(e.what()));
		}
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

void SteamNetworkingSocket::Stop()
{
	bStop = true;
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
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Closed By Peer: %s"), UTF8_TO_TCHAR(info->m_info.m_szEndDebug));
		UE_LOG(LogTemp, Error, TEXT("Connection closed by peer. Reason: %d, Info: %hs"), info->m_info.m_eEndReason, UTF8_TO_TCHAR(info->m_info.m_szEndDebug));
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

void SteamNetworkingSocket::PushUdpFlushMessage(Message& msg)
{
	send_buffer_.Push(msg);
}

