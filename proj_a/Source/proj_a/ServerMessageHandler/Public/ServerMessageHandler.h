#pragma once

#include "CoreMinimal.h"
#include "message.pb.h"
#include "DoubleBuffer.h"
#include <map>

class ADPPlayerController;

DECLARE_DELEGATE_ThreeParams(FServerMessageDelegate, ADPPlayerController*, const Message&, DoubleBuffer&);

class ServerMessageHandler
{
	typedef std::map<Message::MessageTypeCase, FServerMessageDelegate>::iterator MessageHandlerIterator;
	typedef std::map<Message::MessageTypeCase, FServerMessageDelegate> MessageHandlerMap;
public:
	ServerMessageHandler();
	FServerMessageDelegate* HandleMessage(const Message& Msg);
private:
	MessageHandlerMap message_handlers_;
};
