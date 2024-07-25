#pragma once

#include "CoreMinimal.h"
#include "SocketHandler.h"
#include "ANetworkManager.generated.h"

UCLASS(Abstract)
class UANetworkManager : public UActorComponent
{
	GENERATED_BODY()
public:
	void Initialize();
protected:
	UPROPERTY()
	USocketHandler* SocketHandler;
};
