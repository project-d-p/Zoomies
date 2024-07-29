#include "ISocketInterface.h"

UISocketInterface::UISocketInterface()
{
}

UISocketInterface::~UISocketInterface()
{
}

UISocketInterface* UISocketInterface::Clone() const
{
	return nullptr;
}

void UISocketInterface::ActivateServer()
{
}

void UISocketInterface::ActivateClient()
{
}

void UISocketInterface::RecieveData(const TFunction<void(const Message&)>& Callback)
{
}

void UISocketInterface::SendData(Message& Msg)
{
}

void UISocketInterface::SetAsServer()
{
}

void UISocketInterface::SetAsClient()
{
}

void UISocketInterface::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function)
{
}


