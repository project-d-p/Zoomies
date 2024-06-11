#include "ServerChatManager.h"

#include "DPPlayerController.h"

void UServerChatManager::BroadcastChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADPPlayerController* PC = Cast<ADPPlayerController>(*It);
		if (PC)
		{
			PC->ReceiveChatMessage(SenderName, Message);
		}
	}
}
