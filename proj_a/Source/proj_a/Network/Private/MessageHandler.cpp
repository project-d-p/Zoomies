#include "MessageHandler.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

FMessageHandler::FMessageHandler()
{
	// Movement 메시지 처리 함수 등록
	MessageHandlers.Add(Message::kMovement, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		// FNetLogger::GetInstance().LogInfo(TEXT("Movement message received: %s", *FString(Msg.DebugString().c_str())));
		FDataHub::PushEchoDA(Msg);
	}));

	// PlayerPosition 메시지 처리 함수 등록
	MessageHandlers.Add(Message::kActorPosition, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushActorDA(Msg);
	}));

	// Jump 메시지 처리 함수 등록
	MessageHandlers.Add(Message::kJump, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushJumpDA(Msg);
	}));

	MessageHandlers.Add(Message::kGunfire, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushGunfireDA(Msg);
	}));

	MessageHandlers.Add(Message::kAimState, FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushAimStateDA(Msg);
	}));
}

void FMessageHandler::HandleMessage(const Message& Msg)
{
	const FMessageDelegate* foundDelegate = MessageHandlers.Find(Msg.message_type_case());
	if (foundDelegate)
	{
		// UE_LOG(LogNetwork, Warning, TEXT("Found message type: %d"), Msg.message_type_case());
		(void)foundDelegate->ExecuteIfBound(Msg);
	}
	else
	{
		// UE_LOG(LogNetwork, Warning, TEXT("Unknown message type received: %s"), *FString(Msg.DebugString().c_str()));
	}
}
