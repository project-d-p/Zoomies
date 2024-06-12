#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "HAL/Runnable.h"
#include "DoubleBuffer.h"
#include "HAL/RunnableThread.h"

class FClientHandler : public FRunnable
{
public:
	FClientHandler(FSocket* client_socket, DoubleBuffer& double_buffer);
	FSocket* GetClientSocket() const;
	virtual  ~FClientHandler() override;
private:
	virtual uint32 Run() override;
private:
	FSocket* client_socket_;
	FRunnableThread* thread_;
	DoubleBuffer& double_buffer_;
};
