#include "ServerChatManager.h"

#include "DPPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "IChatGameState.h"

void UServerChatManager::BroadcastChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	IChatGameState* GS = Cast<IChatGameState>(GetWorld()->GetGameState());
	check(GS && GS->GetChatManager())
	GS->GetChatManager()->ClientReceiveChatMessage(SenderName, Message);
}
