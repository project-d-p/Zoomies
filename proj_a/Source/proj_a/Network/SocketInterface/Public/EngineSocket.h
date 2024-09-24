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
	virtual ~UEngineSocket();
	virtual UISocketInterface* Clone() const;

	virtual void ActivateServer();
	virtual void ActivateClient();

	virtual void RecieveData(const TFunction<void(const Message&)>& Callback);
	virtual void SendData(Message& Msg);
	virtual void SetAsServer();
	virtual void SetAsClient();
	virtual void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function);

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
	FTimerHandle ConnectionTimerHandle;
};
