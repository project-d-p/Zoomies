#include "MessageHandler.h"

#include "FDataHub.h"

DEFINE_LOG_CATEGORY(LogNetwork);

FMessageHandler::FMessageHandler()
{
	// Movement 메시지 처리 함수 등록
	MessageHandlers.Add(Message::kMovement, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		// UE_LOG(LogNetwork, Log, TEXT("Movement message received: %s"), *FString(Msg.DebugString().c_str()));
		FDataHub::PushEchoDA(Msg.movement());
	}));

	// PlayerPosition 메시지 처리 함수 등록
	MessageHandlers.Add(Message::kPlayerPosition, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		// UE_LOG(LogNetwork, Log, TEXT("Player position update received: %s"), *FString(Msg.DebugString().c_str()));
		FDataHub::PushPlayerDA(Msg.player_position());
	}));
}

void FMessageHandler::HandleMessage(const Message& Msg)
{
	const FMessageDelegate* FoundDelegate = MessageHandlers.Find(Msg.message_type_case());
	if (FoundDelegate)
	{
		(void)FoundDelegate->ExecuteIfBound(Msg);
	}
	else
	{
		UE_LOG(LogNetwork, Warning, TEXT("Unknown message type received: %s"), *FString(Msg.DebugString().c_str()));
	}
}
