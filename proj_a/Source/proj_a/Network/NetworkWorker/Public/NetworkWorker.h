#pragma once

#include "CoreMinimal.h"
#include "DoubleBuffer.h"
#include "message.pb.h"
#include "isteamnetworkingsockets.h"
#include "steamnetworkingtypes.h"
#include "steam_api_common.h"
#include "ISocketInterface.h"
#include "NetworkWorker.generated.h"


UCLASS()
class UNetworkWorker : public UObject, public FRunnable
{
	GENERATED_BODY()
public:
	UNetworkWorker();
	void Initialize(ISocketInterface* SocketInterface);
	void SetMessageReceivedCallback(TFunction<void(const Message&)> Callback);
	void SendData(const Message& Data);

	virtual uint32 Run() override;
	virtual void Stop() override;
private:
	void FlushSendMessages();

	bool bIsRunning = true;
	ISocketInterface* SocketInterface = nullptr;
	DoubleBuffer SendBuffer;
	TFunction<void(const Message&)> DataRecieveCallback;
};
