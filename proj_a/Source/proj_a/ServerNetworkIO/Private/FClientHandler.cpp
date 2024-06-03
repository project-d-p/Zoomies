#include "FClientHandler.h"

#include "Marshaller.h"
#include "message.pb.h"

FClientHandler::FClientHandler(FSocket* client_socket, DoubleBuffer& double_buffer)
	: client_socket_(client_socket), double_buffer_(double_buffer)
{
	thread_ = FRunnableThread::Create(this, TEXT("CLIENT HANDLER THREAD"));
}

FClientHandler::~FClientHandler()
{
	if (thread_)
	{
		thread_->Kill(true);
		delete thread_;
	}
	if (client_socket_)
	{
		client_socket_->Close();
		delete client_socket_;
	}
}

uint32 FClientHandler::Run()
{
	const int32 buffer_size = 1024;
	TArray<uint8> data;
	
	data.Reserve(buffer_size);
	data.SetNumZeroed(buffer_size);
	while (client_socket_)
	{
		uint32 size;
		while (client_socket_->HasPendingData(size))
		{
			int32 read = 0;
			client_socket_->Recv(data.GetData(), size, read);
			Message msg = Marshaller::DeserializeMessage(data);
			double_buffer_.Push(msg);
			data.SetNumZeroed(buffer_size);
		}
		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}