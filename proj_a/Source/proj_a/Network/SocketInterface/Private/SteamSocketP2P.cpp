#include "SteamSocketP2P.h"

#include "SteamSocketIP.h"

UISocketInterface* USteamSocketP2P::Clone() const
{
	return NewObject<USteamSocketIP>();
}

void USteamSocketP2P::ActivateServer()
{
	
}

void USteamSocketP2P::ActivateClient()
{
}

USteamSocketP2P::~USteamSocketP2P()
{
	
}
