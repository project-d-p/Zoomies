#include "ISocketFactory.h"

#include "EngineSocket.h"
#include "FNetLogger.h"
#include "ISocketInterface.h"
#include "SteamSocketIP.h"
#include "SteamSocketP2P.h"

UISocketFactory::UISocketFactory()
{
	SocketTypeToSocketInterfaceMap[ENetworkTypeZoomies::SOCKET_STEAM_LAN] = [](UObject* Outer)->UISocketInterface* { return NewObject<USteamSocketIP>(Outer); };
	SocketTypeToSocketInterfaceMap[ENetworkTypeZoomies::SOCKET_STEAM_P2P] = [](UObject* Outer)->UISocketInterface* { return NewObject<USteamSocketP2P>(Outer); };
	SocketTypeToSocketInterfaceMap[ENetworkTypeZoomies::ENGINE_SOCKET] = [](UObject* Outer)->UISocketInterface* { return NewObject<UEngineSocket>(Outer); };
}

UISocketInterface* UISocketFactory::CreateSocketInterface(ENetworkTypeZoomies socketType, UObject* Outer)
{
	UISocketInterface* SocketInterface = SocketTypeToSocketInterfaceMap[socketType](Outer);
	return SocketInterface;
}

UISocketFactory::~UISocketFactory()
{
	SocketTypeToSocketInterfaceMap.clear();
}
