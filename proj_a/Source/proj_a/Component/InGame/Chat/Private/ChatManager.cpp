#include "ChatManager.h"

#include "DPGameModeBase.h"
#include "GameHelper.h"
#include "GameFramework/GameMode.h"

void UChatManager::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	ADPGameModeBase* DPGameMode = UGameHelper::GetInGameMode(GetWorld());
	if (DPGameMode)
	{
		DPGameMode->SendChatToAllClients(SenderName, Message);
	}
	else
	{
		AGameMode* DefaultGameMode = Cast<AGameMode>(GetWorld()->GetAuthGameMode());
		if (DefaultGameMode)
		{
			DefaultGameMode->Broadcast(GetOwner(), Message);
		}
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
	if (ChatUI)
	{
		ChatUI->DestroyComponent();
		ChatUI = InChatUI;
	}
	else
	{
		ChatUI = InChatUI;
	}
}
