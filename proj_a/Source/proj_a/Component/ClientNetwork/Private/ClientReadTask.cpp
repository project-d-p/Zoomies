#include "ClientReadTask.h"

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
			Message msg = Marshaller::DeserializeMessage(data);
			MessageHandler.HandleMessage(msg);		
		}
	}
	return 0;
}
