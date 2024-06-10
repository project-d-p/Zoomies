#include "ServerMessageHandler.h"
#include "DPPlayerController.h"

ServerMessageHandler::ServerMessageHandler()
{
	message_handlers_[Message::kMovement] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, DoubleBuffer& double_buffer)
	{
		player_controller->HandleMovement(msg.movement(), double_buffer);
	});
}

FServerMessageDelegate* ServerMessageHandler::HandleMessage(const Message& Msg)
{
	MessageHandlerIterator found_delegate = message_handlers_.find(Msg.message_type_case());
	if (found_delegate != message_handlers_.end())
	{
		return &(found_delegate->second);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unknown message type received: %s"), *FString(Msg.DebugString().c_str()));
		return nullptr;
	}
}
