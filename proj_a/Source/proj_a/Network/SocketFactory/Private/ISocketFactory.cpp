#include "ISocketFactory.h"
#include "ISocketInterface.h"
#include "SteamSocketIP.h"

TMap<ENetworkTypeZoomies, ISocketInterface*> ISocketFactory::SocketTypeToSocketInterfaceMap = {
	{ENetworkTypeZoomies::SOCKET_STEAM_LAN, new SteamSocketIP()},
};

ISocketInterface* ISocketFactory::CreateSocketInterface(ENetworkTypeZoomies socketType)
{
	ISocketInterface* SocketInterface = SocketTypeToSocketInterfaceMap[socketType]->Clone();
	return SocketInterface;
}

ISocketFactory::~ISocketFactory()
{
	for (auto& SocketInterface : SocketTypeToSocketInterfaceMap)
	{
		delete SocketInterface.Value;
	}
}
