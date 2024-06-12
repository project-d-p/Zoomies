#pragma once

class ClientReadTask : public FRunnable
{
public:
	ClientReadTask(FSocket* socket);
	virtual ~ClientReadTask() override;
private:
	virtual uint32 Run() override;
	FSocket* tcp_socket_ = nullptr;
};

