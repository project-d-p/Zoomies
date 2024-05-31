#include "ChatManager.h"

#include "DPGameModeBase.h"

void UChatManager::ServerSendChatMessage_Implementation(const FString& Message)
{
	if (ADPGameModeBase* GM = Cast<ADPGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		FString SenderName = TEXT("Unknown");
		GM->BroadcastChatMessage(SenderName, Message);
	}
}

bool UChatManager::ServerSendChatMessage_Validate(const FString& Message)
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

void UChatManager::setChatUI(UChatUI* InChatUI)
{
	if (ChatUI == nullptr)
	{
		ChatUI = InChatUI;
	}
}
