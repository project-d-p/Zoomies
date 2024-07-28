#pragma once

#include "CoreMinimal.h"
#include "DoubleBuffer.h"
#include "ESocketType.h"
#include "message.pb.h"
#include <queue>
#include "ANetworkManager.generated.h"

UCLASS(Abstract)
class UANetworkManager : public UObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(ZOOMIES::ESocketType SocketType);
	virtual void OnDataReceived(const Message& Data);
	virtual void SendData(const Message& Data);
	virtual void Shutdown();

	virtual ~UANetworkManager() override;

	std::queue<Message> GetRecievedMessages();	
protected:
	DoubleBuffer RecievedMessageBuffer;
};
