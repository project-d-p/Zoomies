#pragma once

#include "FBaseReceiveTask.h"

class FTcpReceiveTask : public FBaseReceiveTask
{
public:
	FTcpReceiveTask();

	bool InitializeSocket();
	virtual uint32 Run() override;
};
