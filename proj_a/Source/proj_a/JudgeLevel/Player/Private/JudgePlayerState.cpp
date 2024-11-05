#include "JudgePlayerState.h"

#include "Net/OnlineEngineInterface.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

void AJudgePlayerState::RegisterPlayerWithSession(bool bWasFromInvite)
{
	SetSessionName();
	
	Super::UnregisterPlayerWithSession();

	if (GetNetMode() == NM_Client && GetUniqueId().IsValid())
	{
		if (this->SessionName != NAME_None)
		{
			if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), this->SessionName))
			{
				UOnlineEngineInterface::Get()->UnregisterPlayer(GetWorld(), this->SessionName, GetUniqueId());
			}
		}
	}
}

void AJudgePlayerState::UnregisterPlayerWithSession()
{
	SetSessionName();
	
	Super::UnregisterPlayerWithSession();

	if (GetNetMode() == NM_Client && GetUniqueId().IsValid())
	{
		if (this->SessionName != NAME_None)
		{
			if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), this->SessionName))
			{
				UOnlineEngineInterface::Get()->UnregisterPlayer(GetWorld(), this->SessionName, GetUniqueId());
			}
		}
	}
}

void AJudgePlayerState::SetSessionName()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		SessionName = GameInstance->GetSessionName();
	}
}
