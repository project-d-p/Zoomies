#pragma once

#include "CoreMinimal.h"
#include "SteamSocket.h"
#include "Engine/World.h"
#include "SteamSocketP2P.generated.h"

UCLASS()
class USteamSocketP2P : public USteamSocket
{
	GENERATED_BODY()
public:
	virtual UISocketInterface* Clone() const override;
	virtual void ActivateServer() override;
	virtual void ActivateClient() override;
	virtual ~USteamSocketP2P() override;

protected:
	virtual void BeginDestroy() override;

private:
	CSteamID GetHostSteamID();
	void CheckRelayNetworkStatus();
	
	HSteamListenSocket m_ListenSocket;
	HSteamNetConnection m_Connection;
};
