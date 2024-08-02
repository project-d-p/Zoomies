#include "ServerMessageHandler.h"
#include "DPPlayerController.h"

ServerMessageHandler::ServerMessageHandler()
{
	message_handlers_[Message::kMovement] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& delta)
	{
		// player_controller->HandleMovement(msg.movement(), delta);
	});

	message_handlers_[Message::kJump] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		// player_controller->HandleJump(msg.jump());
	});

	message_handlers_[Message::kGunfire] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(player_controller->GetLevelComponent());
		MainLevelComponent->AddGunMessage(msg);
	});

	message_handlers_[Message::kAimState] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(player_controller->GetLevelComponent());
		if (MainLevelComponent == nullptr)
		{
			return ;
		}
		MainLevelComponent->AddAimMessage(msg);
	});

	message_handlers_[Message::kActorPosition] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(player_controller->GetLevelComponent());
		if (MainLevelComponent == nullptr)
		{
			return ;
		}
		MainLevelComponent->HandlePosition(msg.actor_position());
	});

	message_handlers_[Message::kCatch] = FServerMessageDelegate::CreateLambda([](ADPPlayerController* player_controller, const Message& msg, const float& /*delta*/)
	{
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(player_controller->GetLevelComponent());
		if (MainLevelComponent == nullptr)
		{
			return ;
		}
		MainLevelComponent->AddCatchMessage(msg);
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
