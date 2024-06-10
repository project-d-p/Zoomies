#include "ChatManager.h"

#include "DPGameModeBase.h"
#include "GameHelper.h"

void UChatManager::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	ADPGameModeBase* DPGameMode = UGameHelper::GetInGameMode(GetWorld());
	if (DPGameMode)
	{
		DPGameMode->SendChatToAllClients(SenderName, Message);
	}
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

void UChatManager::setChatUI(UChatUI* InChatUI)
{
	if (ChatUI == nullptr)
	{
		ChatUI = InChatUI;
	}
}
