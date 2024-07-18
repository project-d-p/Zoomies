#include "ChatManager.h"

#include "DPGameModeBase.h"
#include "GameHelper.h"

void UChatManager::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	ADPGameModeBase* DPGameMode = UGameHelper::GetInGameMode(GetWorld());
	check(DPGameMode && DPGameMode->GetChatManager())
	DPGameMode->GetChatManager()->BroadcastChatMessage(SenderName, Message);
}

bool UChatManager::ServerSendChatMessage_Validate(const FString& SenderName, const FString& Message)
{
	return !Message.IsEmpty();
}

void UChatManager::ClientReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	check(ChatUI)
	ChatUI->AddChatMessage(SenderName, Message);
}

void UChatManager::setChatUI(UChatUI* InChatUI)
{
	ChatUI = InChatUI;
}
