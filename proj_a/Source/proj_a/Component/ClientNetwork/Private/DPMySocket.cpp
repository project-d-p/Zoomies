#include "DPMySocket.h"

#include "ClientReadTask.h"
#include "ClientWriteTask.h"
#include "FNetLogger.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"


void UMySocket::CreateSocket()
{
	tcp_socket_ = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
}

void UMySocket::Connect(FString ip, uint32 port)
{
	if (!tcp_socket_) return;

	// Parse the IP address
	FIPv4Address ip_address;
	FIPv4Address::Parse(ip, ip_address);

	// Create the endpoint
	FIPv4Endpoint endpoint(ip_address, port);

	// Connect to the server
	if (tcp_socket_->Connect(*endpoint.ToInternetAddr()))
	{
		FNetLogger::EditerLog(FColor::Blue, TEXT("Connected to server"));
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Failed to connect to server"));
	}
}

void UMySocket::RunTask()
{
	tcp_read_thread_ = FRunnableThread::Create(new ClientReadTask(this->tcp_socket_), TEXT("TcpReadThread"));
	tcp_write_thread_ = FRunnableThread::Create(new ClientWriteTask(this->tcp_socket_, this->tcp_write_buffer_), TEXT("TcpWriteThread"));
}

void UMySocket::SendPacket(const Message& msg)
{
	tcp_write_buffer_.Push(msg);
}

UMySocket::~UMySocket()
{
	if (tcp_read_thread_)
	{
		tcp_read_thread_->Kill(true);
		delete tcp_read_thread_;
		tcp_read_thread_ = nullptr;
	}
	if (tcp_write_thread_)
	{
		tcp_write_thread_->Kill(true);
		delete tcp_write_thread_;
		tcp_write_thread_ = nullptr;
	}
	if (tcp_socket_)
	{
		tcp_socket_->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(tcp_socket_);
		tcp_socket_ = nullptr;	
	}
}
