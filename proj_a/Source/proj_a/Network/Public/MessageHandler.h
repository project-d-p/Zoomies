#pragma once

#include "CoreMinimal.h"
#include "Message.pb.h"
#include "Containers/Map.h"
#include "Delegates/Delegate.h"

DECLARE_DELEGATE_OneParam(FMessageDelegate, const Message&);

class UMessageHandler
{
public:
	std::map<Message::MessageTypeCase, FMessageDelegate> MessageHandlers;

	UMessageHandler();
	void HandleMessage(const Message& Msg);
};
