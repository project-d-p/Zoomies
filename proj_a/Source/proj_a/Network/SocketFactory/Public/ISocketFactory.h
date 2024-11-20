#pragma once

#include "CoreMinimal.h"
#include "ENetworkTypeZoomies.h"
#include "ISocketInterface.h"
#include "ISocketFactory.generated.h"

UCLASS()
class UISocketFactory : public UObject
{
	GENERATED_BODY()
public:
	UISocketFactory();
	UISocketInterface* CreateSocketInterface(ENetworkTypeZoomies socketType);
	virtual ~UISocketFactory() override;
private:
	TMap<ENetworkTypeZoomies, TFunction<UISocketInterface*(void)>> SocketTypeToSocketInterfaceMap;
};
