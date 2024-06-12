#include "ClientReadTask.h"

#include "FNetLogger.h"
#include "Marshaller.h"
#include "MessageHandler.h"
#include "Sockets.h"

ClientReadTask::ClientReadTask(FSocket* socket)
	: tcp_socket_(socket)
{
}

ClientReadTask::~ClientReadTask()
{
}

uint32 ClientReadTask::Run()
{
	const int32 buffer_size = 1024;
	TArray<uint8> data;
	FMessageHandler MessageHandler;
	
	data.Reserve(buffer_size);
	data.SetNumZeroed(buffer_size);
	while (tcp_socket_)
	{
		uint32 size;
		while (tcp_socket_->HasPendingData(size))
		{
			int32 read = 0;
			tcp_socket_->Recv(data.GetData(), size, read);
			TArray<uint8> parsed_line;
			Message msg = Marshaller::DeserializeMessage(data);
			MessageHandler.HandleMessage(msg);
			data.SetNumZeroed(buffer_size);
		}
		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}
