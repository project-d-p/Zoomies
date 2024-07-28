#include "ISocketFactory.h"
#include "ISocketInterface.h"
#include "SteamSocketIP.h"

UISocketFactory::UISocketFactory()
{
	SocketTypeToSocketInterfaceMap.Add(ENetworkTypeZoomies::SOCKET_STEAM_LAN, []()->UISocketInterface* { return NewObject<USteamSocketIP>(); });
}

UISocketInterface* UISocketFactory::CreateSocketInterface(ENetworkTypeZoomies socketType)
{
	UISocketInterface* SocketInterface = SocketTypeToSocketInterfaceMap[socketType]();
	return SocketInterface;
}

UISocketFactory::~UISocketFactory()
{
}
