#pragma once

#include "FBaseReceiveTask.h"

class FUdpReceiveTask : public FBaseReceiveTask
{
public:
	FUdpReceiveTask(const FString Ip, int32 Port);
	virtual ~FUdpReceiveTask() override;
	bool InitializeSocket(const FString Ip, int32 Port);
	virtual uint32 Run() override;
};
