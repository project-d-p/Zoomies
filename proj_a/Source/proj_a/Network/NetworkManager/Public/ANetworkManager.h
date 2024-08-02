#pragma once

#include "CoreMinimal.h"
#include "DoubleBuffer.h"
#include "message.pb.h"
#include "ISocketFactory.h"
#include <queue>
#include "ENetworkTypeZoomies.h"
#include "ANetworkManager.generated.h"

UCLASS(Abstract)
class UANetworkManager : public UObject
{
	GENERATED_BODY()
public:
	UANetworkManager();
	virtual void Initialize(ENetworkTypeZoomies SocketType);
	virtual void OnDataReceived(const Message& Data);
	virtual void SendData(const Message& Data);
	virtual void Shutdown();

	// Set the callback for when the game starts
	virtual void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Callback);

	virtual ~UANetworkManager() override;

	std::queue<Message> GetRecievedMessages();	
protected:
	DoubleBuffer RecievedMessageBuffer;

	UPROPERTY()
	UISocketFactory* SocketFactory;
};
