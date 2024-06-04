#include "ServerChatManager.h"

#include "DPPlayerController.h"

void UServerChatManager::BroadcastChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	if (AActor* Owner = GetOwner())
		if (!Owner->HasAuthority())
			return;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADPPlayerController* PC = Cast<ADPPlayerController>(*It);
		if (PC)
		{
			PC->ReceiveChatMessage(SenderName, Message);
		}
	}
}
