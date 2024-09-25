#pragma once

#include "CoreMinimal.h"
#include "SteamSocket.h"
#include "SteamSocketIP.generated.h"

UCLASS()
class USteamSocketIP : public USteamSocket
{
	GENERATED_BODY()
public:
	virtual UISocketInterface* Clone() const;
	virtual void ActivateServer() override;
	virtual void ActivateClient() override;
	virtual ~USteamSocketIP();

protected:
	virtual void BeginDestroy() override;
	
private:
	enum
	{
		SERVER_PORT = 4242,
		SERVER_IPV4_LOCAL = 0x7F000001
	};
	SteamNetworkingConfigValue_t m_ConfigValue;
	SteamNetworkingIPAddr m_LocalAddr;
	
	HSteamListenSocket m_ListenSocket;
	HSteamNetConnection m_Connection;
};
