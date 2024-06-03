#include "ChatManager.h"

#include "DPGameModeBase.h"
#include "DPPlayerController.h"

void UChatManager::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	BroadcastChatMessage(SenderName, Message);
}

bool UChatManager::ServerSendChatMessage_Validate(const FString& SenderName, const FString& Message)
{
	return !Message.IsEmpty();
}

void UChatManager::ClientReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	if (ChatUI)
	{
		ChatUI->AddChatMessage(SenderName, Message);
	}
}

void UChatManager::BroadcastChatMessage_Implementation(const FString& SenderName, const FString& Message)
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

void UChatManager::setChatUI(UChatUI* InChatUI)
{
	if (ChatUI == nullptr)
	{
		ChatUI = InChatUI;
	}
}
