#pragma once

#include "CoreMinimal.h"
#include "DoubleBuffer.h"
#include "message.pb.h"
#include "ISocketInterface.h"
#include "NetworkWorker.generated.h"


UCLASS()
class UNetworkWorker : public UObject, public FRunnable
{
	GENERATED_BODY()
public:
	UNetworkWorker();
	void Initialize(UISocketInterface* SocketInterface);
	void SetMessageReceivedCallback(TFunction<void(const Message&)> Callback);
	void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function);
	
	void SendData(const Message& Data);

	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual ~UNetworkWorker() override;
private:
	void FlushSendMessages();

	bool bIsRunning = true;
	UPROPERTY()
	UISocketInterface* SocketInterface = nullptr;
	DoubleBuffer SendBuffer;
	TFunction<void(const Message&)> DataRecieveCallback;
};
