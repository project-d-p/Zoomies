#pragma once

#include "CoreMinimal.h"
#include "message.pb.h"

/*
 * TODO: Add Children for SteamSockets and WinSockets etc.
 */

class ISocketInterface {
public:
	virtual ~ISocketInterface() = default;
	virtual ISocketInterface* Clone() const = 0;
	virtual void ActivateServer() = 0;
	virtual void ActivateClient() = 0;
	virtual void RecieveData(const TFunction<void(const Message&)>& Callback) = 0;
	virtual void SendData(const Message& Msg) = 0;
	virtual void SetAsServer() = 0;
	virtual void SetAsClient() = 0;
	virtual void SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function) = 0;
};
