#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IChatGameState.generated.h"

class UChatManager;

UINTERFACE(MinimalAPI)
class UChatGameState : public UInterface
{
	GENERATED_BODY()
};

class IChatGameState
{
	GENERATED_BODY()
public:
	virtual UChatManager* GetChatManager() const = 0;
};
