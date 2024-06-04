#pragma once

#include "CoreMinimal.h"
#include "steam_api.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

class ListenSocket : public FRunnable
{
public:
	ListenSocket();
	~ListenSocket();
private:
	// For Local Testing
	FSocket* listen_socket_ = nullptr;
	FRunnableThread* thread_ = nullptr;
	// For Steam
	// HSteamListenSocket steam_listen_socket_;
	
};
