#pragma once

#include "CoreMinimal.h"
#include "ANetworkManager.h"
#include "NetworkWorker.h"
#include "ServerNetworkManager.generated.h"

UCLASS()
class UServerNetworkManager : public UANetworkManager
{
	GENERATED_BODY()

public:
	virtual void Initialize(ZOOMIES::ESocketType SocketType) override;
	virtual void OnDataReceived(const Message& Data) override;
	virtual void SendData(const Message& Data) override;
	virtual void Shutdown() override;
	virtual void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Callback) override;
	virtual ~UServerNetworkManager() override;
	
private:
	UPROPERTY()
	UNetworkWorker* Worker = nullptr;
	FRunnableThread* WorkerThread = nullptr;
};

