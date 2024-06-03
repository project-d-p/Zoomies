#include "ListenSocket.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include <exception>

FListenSocketRunnable::FListenSocketRunnable(bool& bis_game_started)
	: bis_game_started_(bis_game_started)
{
	if (this->CreateListenSocket(this->ip_, this->port_))
		this->RunListenSocket();
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create listen socket."));
		throw std::exception("Failed to create listen socket.");
	}
}

FListenSocketRunnable::~FListenSocketRunnable()
{
	if (thread_)
	{
		thread_->Kill(true);
		delete thread_;
	}
	if (listen_socket_)
	{
		listen_socket_->Close();
		delete listen_socket_;
	}
}

uint32 FListenSocketRunnable::Run()
{
	while (listen_socket_)
	{
		bool pending;
		listen_socket_->HasPendingConnection(pending);
		if (pending)
		{
			double_buffers_.emplace_back();
			FSocket* new_connection = listen_socket_->Accept(TEXT("CLIENT SOCKET"));
			if (new_connection)
			{
				this->client_handlers_.push_back(new FClientHandler(new_connection, double_buffers_.back()));
			}
		}
		if (this->GetNumOfClients() == NUM_OF_MAX_CLIENTS)
		{
			bis_game_started_ = true;	
		}
		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}

bool FListenSocketRunnable::CreateListenSocket(const FString& ip, const int32 port)
{
	FIPv4Address address;
	FIPv4Address::Parse(ip, address);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(address.Value);
	addr->SetPort(4242);
	
	listen_socket_ = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("DEFAULT"), false);
	listen_socket_->SetReuseAddr(true);
	listen_socket_->SetNonBlocking(true);

	bool b_bind = listen_socket_->Bind(*addr);
	bool b_listen = listen_socket_->Listen(8);

	if (!b_bind && !b_listen)
	{
		UE_LOG(LogTemp, Log, TEXT("LISTEN SOCKET FAILED"));
		return false;	
	}
	return true;
}

void FListenSocketRunnable::RunListenSocket()
{
	this->thread_ = FRunnableThread::Create(this, TEXT("ListenSocketThread"));
}

int FListenSocketRunnable::GetNumOfClients() const
{
	return this->client_handlers_.size();
}

void FListenSocketRunnable::FillMessageQueue(MessageQueue_t& message_queue_)
{
	for (auto& buffer : double_buffers_)
	{
		buffer.Swap();
		while (!buffer.Empty())
			message_queue_.push(buffer.Pop());
	}
}