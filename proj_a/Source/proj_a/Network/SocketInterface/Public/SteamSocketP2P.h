#pragma once

#include "SteamSocket.h"

class SteamSocketP2P : public SteamSocket
{
public:
	virtual ISocketInterface* Clone() const;
	virtual void ActivateServer() override;
	virtual void ActivateClient() override;
	virtual ~SteamSocketP2P();

private:
	
};
