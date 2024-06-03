#include "MessageHandler.h"
#include "FDataHub.h"
#include "FNetLogger.h"

FMessageHandler::FMessageHandler()
{
	// Movement 메시지 처리 함수 등록
	MessageHandlers.Add(Message::kMovement, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		// FNetLogger::GetInstance().LogInfo(TEXT("Movement message received: %s", *FString(Msg.DebugString().c_str())));
		FDataHub::PushEchoDA(Msg.movement());
	}));

	// PlayerPosition 메시지 처리 함수 등록
	MessageHandlers.Add(Message::kPlayerPosition, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		// FNetLogger::GetInstance().LogInfo(TEXT("Player position update received: %s", *FString(Msg.DebugString().c_str())));
		FDataHub::PushPlayerDA(Msg.player_position());
	}));
}

void FMessageHandler::HandleMessage(const Message& Msg)
{
	const FMessageDelegate* foundDelegate = MessageHandlers.Find(Msg.message_type_case());
	if (foundDelegate)
	{
		(void)foundDelegate->ExecuteIfBound(Msg);
	}
	else
	{
		UE_LOG(LogNetwork, Warning, TEXT("Unknown message type received: %s"), *FString(Msg.DebugString().c_str()));
	}
}
