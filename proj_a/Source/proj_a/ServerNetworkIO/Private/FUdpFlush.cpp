#include "FUdpFlush.h"
#include "Marshaller.h"
#include "ListenSocket.h"

FUdpFlushRunnable::FUdpFlushRunnable(FListenSocketRunnable* listen_socket_runnable)
	: listen_socket_runnable_(listen_socket_runnable)
{
}

uint32 FUdpFlushRunnable::Run()
{
	while (true)
	{
		std::queue<Message> ms_queue_ = listen_socket_runnable_->GetUdpSendBuffer().GetReadBuffer();
		while (!ms_queue_.empty())
		{
			Message msg = ms_queue_.front();
			ms_queue_.pop();
			this->BroadCastMessage(msg);
		}
	}
}

void FUdpFlushRunnable::BroadCastMessage(const Message& msg) const
{
	int32 byte_send = 0;
	TArray<uint8> data = Marshaller::SerializeMessage(msg);
	for (auto client_handler : listen_socket_runnable_->client_handlers_)
	{
		client_handler->GetClientSocket()->Send(data.GetData(), data.Num(), byte_send);
	}
}

