#pragma once
#include "SteamSocket.h"

class SteamSocketIP : public SteamSocket
{
public:
	virtual ISocketInterface* Clone() const;
	virtual void ActivateServer() override;
	virtual void ActivateClient() override;
	virtual ~SteamSocketIP();
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
