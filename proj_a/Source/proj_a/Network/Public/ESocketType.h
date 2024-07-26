#pragma once

#include "CoreMinimal.h"
#include "ESocketType.generated.h"

UENUM()
enum ESocketType
{
	SOCKET_NONE,
	SOCKET_STEAM_LAN,
	SOCKET_STEAM_P2P,
};
