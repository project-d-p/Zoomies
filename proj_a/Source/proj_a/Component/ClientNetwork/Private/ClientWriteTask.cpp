#include "ClientWriteTask.h"

#include "Marshaller.h"
#include "Sockets.h"

ClientWriteTask::ClientWriteTask(FSocket* socket, DoubleBuffer& buffer)
	: tcp_socket_(socket)
	, tcp_write_buffer_(buffer)
{
}

ClientWriteTask::~ClientWriteTask()
{
}

uint32 ClientWriteTask::Run()
{
	while (true)
	{
		std::queue<Message> messages = tcp_write_buffer_.GetReadBuffer();
		tcp_write_buffer_.Swap();
		while (!messages.empty())
		{
			Message message = messages.front();
			messages.pop();
			TArray<uint8> data = Marshaller::SerializeMessage(message);
			int32 sent = 0;
			tcp_socket_->Send(data.GetData(), data.Num(), sent);
		}
	}
	return 0;	
}



