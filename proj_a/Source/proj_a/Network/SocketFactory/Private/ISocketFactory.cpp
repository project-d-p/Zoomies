#include "ISocketFactory.h"
#include "ISocketInterface.h"
#include "SteamSocketIP.h"

TMap<ZOOMIES::ESocketType, ISocketInterface*> ISocketFactory::SocketTypeToSocketInterfaceMap = {
	{ZOOMIES::ESocketType::SOCKET_STEAM_LAN, new SteamSocketIP()},
};

ISocketInterface* ISocketFactory::CreateSocketInterface(ZOOMIES::ESocketType socketType)
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
