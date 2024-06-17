#include "ClientSocket.h"

#include "FNetLogger.h"
#include "Marshaller.h"
#include "MessageHandler.h"
#include "SteamNetworkingSocket.h"

UClientSocket::UClientSocket()
	: server_identity_(), virtual_port(0), connection_(0)
{
}

void UClientSocket::Connect(const char* /*ip*/, uint16 port)
{
	// My Steam ID[deulee] : 76561199085093557 for Local Test
	// server_identity_.SetSteamID64(76561199085093557);
	//
	// connection_ = SteamNetworkingSockets()->ConnectP2P(server_identity_, virtual_port, 0, nullptr);

	server_address_.Clear();
	// local address

	server_address_.SetIPv4(0x7f000001, port);

	opt_.m_eValue = k_ESteamNetworkingConfig_IP_AllowWithoutAuth;
	opt_.m_eDataType = k_ESteamNetworkingConfig_Int32;
	opt_.m_val.m_int32 = 1;
	connection_ = SteamNetworkingSockets()->ConnectByIPAddress(server_address_, 1, &opt_);
	if (connection_ == k_HSteamNetConnection_Invalid)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to connect to server"));
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Failed to connect to server"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Connected to server"));
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Connected to Server"));
	}
}

void UClientSocket::RunTask()
{
	this_thread_ = FRunnableThread::Create(this, TEXT("ClientSocket"));
}

void UClientSocket::AsyncSendPacket(const Message& msg)
{
	send_buffer_.Push(msg);
}

uint32 UClientSocket::Run()
{
	while (true)
	{
		// SteamAPI_RunCallbacks();
		this->HandleRecieveMessages();
		this->HandleSendMessages();
	}
}

UClientSocket::~UClientSocket()
{
	if (this_thread_)
	{
		this_thread_->Kill(true);
		delete this_thread_;
		this_thread_ = nullptr;
	}
	if (connection_ != k_HSteamNetConnection_Invalid)
	{
		SteamNetworkingSockets()->CloseConnection(connection_, 0, nullptr, false);
	}
}

void UClientSocket::HandleRecieveMessages()
{
	const int n_max_message = 10;
	SteamNetworkingMessage_t* pp_message[n_max_message];
	int n_messages = SteamNetworkingSockets()->ReceiveMessagesOnConnection(connection_, pp_message, n_max_message);

	if (n_messages == 0)
	{
		return ;
	}
	if (n_messages < 0)
	{
		return ;
	}

	FMessageHandler message_handler;
	for (int i = 0; i < n_messages; ++i)
	{
		char* msg = static_cast<char*>(const_cast<void*>(pp_message[i]->GetData()));
		const int size = pp_message[i]->GetSize();
		
		TArray<uint8> data(reinterpret_cast<uint8*>(msg), size);
		Message ret_msg = Marshaller::DeserializeMessage(data);
		pp_message[i]->Release();
		// recieve_buffer_.Push(ret_msg);
		message_handler.HandleMessage(ret_msg);
		data.SetNumZeroed(data.Num());
	}
}

void UClientSocket::HandleSendMessages()
{
	send_buffer_.Swap();
	std::queue<Message> send_buffer = send_buffer_.GetReadBuffer();
	while (!send_buffer.empty())
	{
		Message msg = send_buffer.front();
		TArray<uint8> data = Marshaller::SerializeMessage(msg);
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Size of message: %d"), data.Num());
		SteamNetworkingSockets()->SendMessageToConnection(connection_, data.GetData(), data.Num(), k_nSteamNetworkingSend_Unreliable, nullptr);
		send_buffer.pop();
		data.SetNumZeroed(data.Num());
	}
}
