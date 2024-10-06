#pragma once

#include "CoreMinimal.h"
#include "ENetworkTypeZoomies.generated.h"

UENUM(BlueprintType)
enum class ENetworkTypeZoomies : uint8
{
	NONE,
	SOCKET_STEAM_LAN,
	SOCKET_STEAM_P2P,
	ENGINE_SOCKET,
};