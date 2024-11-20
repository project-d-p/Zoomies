#pragma once

#include "CoreMinimal.h"
#include "ISocketInterface.h"
#include "EngineSocket.generated.h"

UCLASS()
class UEngineSocket : public UISocketInterface
{
	GENERATED_BODY()
public:
	UEngineSocket();
	virtual ~UEngineSocket() override;
	virtual UISocketInterface* Clone() const override;

	virtual void ActivateServer() override;
	virtual void ActivateClient() override;

	virtual void RecieveData(const TFunction<void(const Message&)>& Callback) override;
	virtual void SendData(Message& Msg) override;
	virtual void SetAsServer() override;
	virtual void SetAsClient() override;
	virtual void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function) override;

protected:
	virtual void BeginDestroy() override;
	
private:
	void AddConnection(FSocket* Connection);
	void CloseConnection(FSocket* Connection);
	
	void OnServerCallback();
	void OnClientCallback();
	
	void HandleIncomingConnections();
	void HandleIncomingData(FSocket* Socket, const TFunction<void(const Message&)>& Callback);

	enum
	{
		SERVER_PORT = 4242,
	};
	FSocket* ListenSocket;
	FSocket* ClientSocket;

	TArray<FSocket*> Connections;
	TFunction<void()> GameStartCallback;
	int MaxClients;
	bool bIsServer;
};
