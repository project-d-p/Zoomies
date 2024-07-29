#include "ISocketFactory.h"
#include "ISocketInterface.h"
#include "SteamSocketIP.h"
#include "SteamSocketP2P.h"

UISocketFactory::UISocketFactory()
{
	SocketTypeToSocketInterfaceMap.Add(ENetworkTypeZoomies::SOCKET_STEAM_LAN, []()->UISocketInterface* { return NewObject<USteamSocketIP>(); });
	SocketTypeToSocketInterfaceMap.Add(ENetworkTypeZoomies::SOCKET_STEAM_P2P, []()->UISocketInterface* { return NewObject<USteamSocketP2P>(); });
}

UISocketInterface* UISocketFactory::CreateSocketInterface(ENetworkTypeZoomies socketType)
{
	UISocketInterface* SocketInterface = SocketTypeToSocketInterfaceMap[socketType]();
	return SocketInterface;
}

UISocketFactory::~UISocketFactory()
{
}
