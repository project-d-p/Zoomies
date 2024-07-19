#include "ChatManager.h"

void UChatManager::ClientReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	check(ChatUI)
	ChatUI->AddChatMessage(SenderName, Message);
}

void UChatManager::setChatUI(UChatUI* InChatUI)
{
	ChatUI = InChatUI;
}
