#pragma once

#include "CoreMinimal.h"
#include "ChatUI.h"
#include "Components/ActorComponent.h"
#include "ChatManager.generated.h"

UCLASS()
class PROJ_A_API UChatManager : public UActorComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);
	
	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FString& SenderName, const FString& Message);

	UFUNCTION()
	void setChatUI(UChatUI* InChatUI);
private:
	UPROPERTY()
	UChatUI* ChatUI = nullptr;
};
