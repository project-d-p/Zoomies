#pragma once

#include "CoreMinimal.h"
#include "ESocketType.h"

class ISocketInterface;

class ISocketFactory
{
public:
	static ISocketInterface* CreateSocketInterface(ZOOMIES::ESocketType socketType);
	~ISocketFactory();
private:
	static TMap<ZOOMIES::ESocketType, ISocketInterface*> SocketTypeToSocketInterfaceMap;
};
