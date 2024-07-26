#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IChatGameMode.generated.h"

class UServerChatManager;

UINTERFACE(MinimalAPI)
class UChatGameMode : public UInterface
{
	GENERATED_BODY()
};

class IChatGameMode
{
	GENERATED_BODY()

public:
	virtual UServerChatManager* GetChatManager() const = 0;
};