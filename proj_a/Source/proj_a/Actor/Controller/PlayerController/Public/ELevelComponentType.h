#pragma once

#include "CoreMinimal.h"
#include "ELevelComponentType.generated.h"

UENUM()
enum class ELevelComponentType
{
	MAIN = 0,
	JUDGE = 1,
	RESULT = 2,
	LOBBY = 3,
	NONE = 4
};
