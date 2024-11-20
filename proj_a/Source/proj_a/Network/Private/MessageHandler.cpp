#include "MessageHandler.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

UMessageHandler::UMessageHandler()
{
	// Movement 메시지 처리 함수 등록
	MessageHandlers[Message::kMovement] = FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushEchoDA(Msg);
	});

	// PlayerPosition 메시지 처리 함수 등록
	MessageHandlers[Message::kActorPosition] = FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushActorDA(Msg);
	});

	// Jump 메시지 처리 함수 등록
	MessageHandlers[Message::kJump] = FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushJumpDA(Msg);
	});

	MessageHandlers[Message::kGunfire] = FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushGunfireDA(Msg);
	});

	MessageHandlers[Message::kAimState] = FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushAimStateDA(Msg);
	});

	MessageHandlers[Message::kMonsterPosition] = FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushMonsterDA(Msg);
	});

	MessageHandlers[Message::kCatch] = FMessageDelegate::CreateLambda([](const Message& Msg)
	{
		FDataHub::PushCatachDA(Msg);
	});
}

void UMessageHandler::HandleMessage(const Message& Msg)
{
	static int count = 0;
	if (MessageHandlers.contains(Msg.message_type_case()))
	{
		const FMessageDelegate foundDelegate = MessageHandlers[Msg.message_type_case()];
		
		// if (count == 500)
		// {
		// 	FString DebuggedString = FString(Msg.DebugString().c_str());
		// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Local Player Position : %s"), *DebuggedString);
		// 	count = 0;
		// }
		// count++;
		// FNetLogger::EditerLog(FColor::Cyan, TEXT("Local Player Position : %d"), count++);
		// UE_LOG(LogNetwork, Warning, TEXT("Found message type: %d"), Msg.message_type_case());
		(void)foundDelegate.ExecuteIfBound(Msg);
	}
	else
	{
		// UE_LOG(LogNetwork, Warning, TEXT("Unknown message type received: %s"), *FString(Msg.DebugString().c_str()));
	}
}
