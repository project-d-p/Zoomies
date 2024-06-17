#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "DoubleBuffer.h"
#include "message.pb.h"
#include "FUdpFlush.h"
#include <queue>
#include <utility>
#include <vector>
#include "FNetLogger.h"
#include "isteamnetworkingsockets.h"
#include "isteamnetworkingutils.h"
#include "steamnetworkingtypes.h"

class SteamNetworkingSocket : public FRunnable
{
	typedef std::queue<Message> MessageQueue_t;

public:
	SteamNetworkingSocket();
	virtual ~SteamNetworkingSocket() override;
	std::queue<Message> GetReadBuffer();
	bool IsGameStarted() const;
	// void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info);
	
	STEAM_CALLBACK(SteamNetworkingSocket, OnSteamNetConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
protected:
	virtual uint32 Run() override;

private:
	void RecieveMessages();
	void FlushSendMessages();

private:
	enum { MAX_CLIENTS = 1 };
	int n_local_virtual_port = 0;
	SteamNetworkingConfigValue_t opt_;
	HSteamListenSocket steam_listen_socket_;
	HSteamNetPollGroup poll_group_;
	DoubleBuffer recieve_buffer_;
	DoubleBuffer send_buffer_;
	std::vector<HSteamNetConnection> steam_connections_;
	FRunnableThread* this_thread_ = nullptr;
	bool b_is_game_stated = false;
	int num_clients_ = 0;
	
	SteamNetworkingIPAddr local_address_;
};
