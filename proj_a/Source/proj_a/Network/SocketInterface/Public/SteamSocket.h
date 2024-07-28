#pragma once

#include "DoubleBuffer.h"
#include "ISocketInterface.h"
#include "message.pb.h"
#include "isteamnetworkingsockets.h"
#include "steamnetworkingtypes.h"
#include "steam_api_common.h"

class SteamSocket :	public ISocketInterface		
{
public:
	SteamSocket();
	virtual ISocketInterface* Clone() const = 0;
	virtual void ActivateServer() = 0;
	virtual void ActivateClient() = 0;
	virtual void RecieveData(const TFunction<void(const Message&)>& Callback) override;
	virtual void SendData(const Message& Msg) override;
	virtual void SetAsServer() override;
	virtual void SetAsClient() override;
	virtual ~SteamSocket() override;

protected:
	/* CallBack on Network Event */
	CCallback<SteamSocket, SteamNetConnectionStatusChangedCallback_t> OnSteamNetConnectionStatusChanged;
	void OnServerCallBack(SteamNetConnectionStatusChangedCallback_t* pParam);
	void OnClientCallBack(SteamNetConnectionStatusChangedCallback_t* pParam);
	
	void AddConnection(HSteamNetConnection Connection);
	void CloseConnection(HSteamNetConnection Connection);
	
	HSteamNetPollGroup PollGroup;
	TArray<HSteamNetConnection> Connections;
};

// 어떻게 하면 CALLBACK에서 클라이언트와 서버의 구현을 다르게 할 수 있을까?