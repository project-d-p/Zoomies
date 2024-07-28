#pragma once

#include "CoreMinimal.h"
#include "ENetworkTypeZoomies.h"

class ISocketInterface;

class ISocketFactory
{
public:
	static ISocketInterface* CreateSocketInterface(ENetworkTypeZoomies socketType);
	~ISocketFactory();
private:
	static TMap<ENetworkTypeZoomies, ISocketInterface*> SocketTypeToSocketInterfaceMap;
};
