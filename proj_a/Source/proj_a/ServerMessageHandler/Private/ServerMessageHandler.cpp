#include "ServerMessageHandler.h"
#include "DPPlayerController.h"

ServerMessageHandler::ServerMessageHandler()
{
	message_handlers_[Message::kMovement] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& delta)
	{
		player_controller->HandleMovement(msg.movement(), delta);
	});

	message_handlers_[Message::kJump] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		player_controller->HandleJump(msg.jump());
	});

	message_handlers_[Message::kGunfire] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		player_controller->HandleFire(msg);
	});

	message_handlers_[Message::kAimState] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		player_controller->HandleAim(msg.aim_state());
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
