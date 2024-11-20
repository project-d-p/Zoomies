#pragma once

#include "CoreMinimal.h"
#include "ISocketInterface.h"
#include "message.pb.h"
#include "isteamnetworkingsockets.h"
#include "steamnetworkingtypes.h"
#include "steam_api_common.h"
#include "SteamSocket.generated.h"

UCLASS()
class USteamSocket : public UISocketInterface		
{
	GENERATED_BODY()
public:
	USteamSocket();
	virtual UISocketInterface* Clone() const override;
	
	virtual void ActivateServer() override;
	virtual void ActivateClient() override;
	
	virtual void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function) override;
	virtual void SetAsServer() override;
	virtual void SetAsClient() override;
	
	virtual void RecieveData(const TFunction<void(const Message&)>& Callback) override;
	virtual void SendData(Message& Msg) override;
	
	virtual ~USteamSocket() override;

	STEAM_CALLBACK(USteamSocket, OnSteamNetConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
protected:
	/* CallBack on Network Event */
	// CCallback<USteamSocket, SteamNetConnectionStatusChangedCallback_t> OnSteamNetConnectionStatusChanged;
	// void OnNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pParam);
	void OnServerCallBack(SteamNetConnectionStatusChangedCallback_t* pParam);
	void OnClientCallBack(SteamNetConnectionStatusChangedCallback_t* pParam);
	
	void AddConnection(HSteamNetConnection Connection);
	void CloseConnection(HSteamNetConnection Connection);

	virtual void BeginDestroy() override;
	
	HSteamNetPollGroup PollGroup;
	TArray<HSteamNetConnection> Connections;
	TFunction<void()> GameStartCallback;
	int MaxClients;
	bool bIsServer;
};

// 어떻게 하면 CALLBACK에서 클라이언트와 서버의 구현을 다르게 할 수 있을까?
