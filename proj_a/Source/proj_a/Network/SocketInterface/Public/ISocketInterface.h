#pragma once

#include "CoreMinimal.h"
#include "message.pb.h"
#include "ISocketInterface.generated.h"

/*
 * TODO: Add Children for SteamSockets and WinSockets etc.
 */

UCLASS()
class UISocketInterface : public UObject {
	GENERATED_BODY()
public:
	UISocketInterface();
	virtual ~UISocketInterface();
	virtual UISocketInterface* Clone() const;

	virtual void ActivateServer();
	virtual void ActivateClient();
	
	virtual void RecieveData(const TFunction<void(const Message&)>& Callback);
	virtual void SendData(Message& Msg);
	virtual void SetAsServer();
	virtual void SetAsClient();
	virtual void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function);
};
