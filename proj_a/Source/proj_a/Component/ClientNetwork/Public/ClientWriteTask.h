#pragma once
#include "DoubleBuffer.h"

class ClientWriteTask : public FRunnable
{
public:
	ClientWriteTask(FSocket* socket, DoubleBuffer& buffer);
	virtual ~ClientWriteTask() override;
private:
	virtual uint32 Run() override;
	FSocket* tcp_socket_ = nullptr;
	DoubleBuffer& tcp_write_buffer_;
};

