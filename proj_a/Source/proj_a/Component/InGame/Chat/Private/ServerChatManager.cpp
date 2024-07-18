#include "ServerChatManager.h"

#include "DPPlayerController.h"

void UServerChatManager::BroadcastChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADPPlayerController* PC = Cast<ADPPlayerController>(*It);
		check(PC && PC->GetChatManager())
		PC->GetChatManager()->ClientReceiveChatMessage(SenderName, Message);
	}
}
