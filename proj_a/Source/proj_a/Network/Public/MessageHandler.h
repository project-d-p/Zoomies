#pragma once

#include "CoreMinimal.h"
#include "Message.pb.h"
#include "Containers/Map.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);

DECLARE_DELEGATE_OneParam(FMessageDelegate, const Message&);

class FMessageHandler
{
public:
	TMap<Message::MessageTypeCase, FMessageDelegate> MessageHandlers;

	FMessageHandler();
	void HandleMessage(const Message& Msg);
};
