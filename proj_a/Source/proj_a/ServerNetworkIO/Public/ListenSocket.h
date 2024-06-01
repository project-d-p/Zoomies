#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "FClientHandler.h"
#include "DoubleBuffer.h"
#include "message.pb.h"
#include "MessageQueueFilter.h"
#include <queue>
#include <utility>
#include <vector>

class FListenSocketRunnable : public FRunnable
{
	typedef std::pair<std::string, Message> message_t;
	typedef std::priority_queue<Message, std::vector<Message>, MessageQueueFilter> MessageQueue_t;
public:
	FListenSocketRunnable(bool& is_game_started);

	void FillMessageQueue(MessageQueue_t& message_queue_);
	
	virtual ~FListenSocketRunnable() override;
	
private:
	virtual uint32 Run() override;
	bool CreateListenSocket(const FString& ip, const int32 port);
	void RunListenSocket();
	int GetNumOfClients() const;
	
private:
	enum { NUM_OF_MAX_CLIENTS = 1 };
	// Default IP and Port
	FString ip_ = "127.0.0.1";
	int32 port_ = 4242;

	// For Local Testing
	bool& bis_game_started_;
	FSocket* listen_socket_ = nullptr;
	FRunnableThread* thread_ = nullptr;

	std::vector<FClientHandler*> client_handlers_;
	std::vector<DoubleBuffer> double_buffers_;

	// For Steam
	// HSteamListenSocket steam_listen_socket_;
};
