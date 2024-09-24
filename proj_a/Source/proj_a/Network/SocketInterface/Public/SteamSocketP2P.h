#pragma once

#include "CoreMinimal.h"
#include "SteamSocket.h"
#include "SteamSocketP2P.generated.h"

UCLASS()
class USteamSocketP2P : public USteamSocket
{
	GENERATED_BODY()
public:
	virtual UISocketInterface* Clone() const;
	virtual void ActivateServer() override;
	virtual void ActivateClient() override;
	virtual ~USteamSocketP2P();

protected:
	virtual void BeginDestroy() override;

private:
	CSteamID GetHostSteamID();
	void CheckRelayNetworkStatus();
	
	HSteamListenSocket m_ListenSocket;
	HSteamNetConnection m_Connection;
};
