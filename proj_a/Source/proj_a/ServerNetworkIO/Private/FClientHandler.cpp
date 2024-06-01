#include "FClientHandler.h"

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
	while (client_socket_)
	{
		uint32 size;
		while (client_socket_->HasPendingData(size))
		{
			uint8* data = new uint8[size];
			int32 read = 0;
			client_socket_->Recv(data, size, read);
			if (read > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("CLIENT: %s"), ANSI_TO_TCHAR((char*)data));
			}
			delete[] data;
		}
		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}