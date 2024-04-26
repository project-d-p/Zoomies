#pragma once

#include "FBaseSendTask.h"

class FUdpSendTask : public FBaseSendTask
{
public:
	FUdpSendTask();
	// virtual ~FUdpSendTask();

	virtual uint32 Run() override;
	// virtual void Stop() override;
};
