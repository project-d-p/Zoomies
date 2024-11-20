#pragma once

#include "CoreMinimal.h"
#include "ANetworkManager.h"
#include "MessageHandler.h"
#include "NetworkWorker.h"
#include "ClientNetworkManager.generated.h"

UCLASS()
class UClientNetworkManager : public UANetworkManager
{
	GENERATED_BODY()
public:
	UClientNetworkManager();
	virtual void Initialize(ENetworkTypeZoomies SocketType) override;
	virtual void OnDataReceived(const Message& Data) override;
	virtual void SendData(const Message& Data) override;
	virtual void Shutdown() override;
	virtual ~UClientNetworkManager() override;
	
private:
	UPROPERTY()
	UNetworkWorker* Worker = nullptr;
	FRunnableThread* WorkerThread = nullptr;
	UMessageHandler MessageHandler;
};
