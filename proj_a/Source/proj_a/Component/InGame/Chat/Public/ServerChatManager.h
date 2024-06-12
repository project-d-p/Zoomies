#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerChatManager.generated.h"

UCLASS()
class PROJ_A_API UServerChatManager : public UActorComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(NetMulticast, Reliable)
	void BroadcastChatMessage(const FString& SenderName, const FString& Message);
};
