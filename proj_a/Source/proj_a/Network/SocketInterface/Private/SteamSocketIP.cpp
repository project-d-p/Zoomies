#include "SteamSocketIP.h"

#include "FNetLogger.h"

UISocketInterface* USteamSocketIP::Clone() const
{
	return NewObject<USteamSocketIP>();
}

void USteamSocketIP::ActivateServer()
{
	m_LocalAddr.Clear();
	m_LocalAddr.m_port = SERVER_PORT;
	
	m_ConfigValue.m_eValue = k_ESteamNetworkingConfig_IP_AllowWithoutAuth;
	m_ConfigValue.m_eDataType = k_ESteamNetworkingConfig_Int32;
	m_ConfigValue.m_val.m_int32 = 1;

	m_ListenSocket = SteamNetworkingSockets()->CreateListenSocketIP(m_LocalAddr, 1, &m_ConfigValue);
	if (m_ListenSocket == k_HSteamListenSocket_Invalid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create listen socket."));
		check(false)
	}
}

void USteamSocketIP::ActivateClient()
{
	m_LocalAddr.Clear();

	m_LocalAddr.SetIPv4(SERVER_IPV4_LOCAL, SERVER_PORT);

	m_ConfigValue.m_eValue = k_ESteamNetworkingConfig_IP_AllowWithoutAuth;
	m_ConfigValue.m_eDataType = k_ESteamNetworkingConfig_Int32;
	m_ConfigValue.m_val.m_int32 = 1;

	m_Connection = SteamNetworkingSockets()->ConnectByIPAddress(m_LocalAddr, 1, &m_ConfigValue);
	if (m_Connection == k_HSteamNetConnection_Invalid)
	{
		FNetLogger::LogError(TEXT("Failed to connect to server"));
		check(false);
	}
	else
	{
		FNetLogger::LogError(TEXT("Connected to server"));
		AddConnection(m_Connection);
	}
}

USteamSocketIP::~USteamSocketIP()
{
	if (m_ListenSocket != k_HSteamListenSocket_Invalid)
	{
		SteamNetworkingSockets()->CloseListenSocket(m_ListenSocket);
	}
	if (m_Connection != k_HSteamNetConnection_Invalid)
	{
		SteamNetworkingSockets()->CloseConnection(m_Connection, 0, nullptr, false);
	}
}
