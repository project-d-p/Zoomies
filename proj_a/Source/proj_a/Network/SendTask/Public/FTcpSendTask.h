#pragma once

#include "FBaseSendTask.h"

class FTcpSendTask : public FBaseSendTask
{
public:
	FTcpSendTask();
	// virtual ~FTcpSendTask();

	virtual uint32 Run() override;
	// virtual void Stop() override;
};
