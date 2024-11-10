#include "JudgePlayerState.h"

#include "Net/OnlineEngineInterface.h"
#include "proj_a/GameInstance/GI_Zoomies.h"


void AJudgePlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);

	ADPPlayerState* DPS = Cast<ADPPlayerState>(NewPlayerState);
	if (DPS)
	{
		if (this->PlayerScoreData)
		{
			DPS->SetPlayerScoreData(this->PlayerScoreData);
		}
	}
}

void AJudgePlayerState::RegisterPlayerWithSession(bool bWasFromInvite)
{
	SetSessionName();

	// Super is not Working Acturally Because Engine Uses Defaults PlayerState so NAME_GameSession is used, not the SessionName
	// Super::RegisterPlayerWithSession(bWasFromInvite);
	
	if (GetNetMode() != NM_Standalone)
	{
		if (GetUniqueId().IsValid()) // May not be valid if this is was created via DebugCreatePlayer
		{
			// Register the player as part of the session
			if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), this->SessionName))
			{
				UOnlineEngineInterface::Get()->RegisterPlayer(GetWorld(), this->SessionName, GetUniqueId(), bWasFromInvite);
			}
		}
	}
}

void AJudgePlayerState::UnregisterPlayerWithSession()
{
	SetSessionName();
	/*
		* in PlauerController.cpp
		* // copy PlayerState data
		if (OldPC->PlayerState)
		{
		OldPC->PlayerState->Reset();
		OldPC->PlayerState->SeamlessTravelTo(PlayerState);
	
		//@fixme: need a way to replace PlayerStates that doesn't cause incorrect "player left the game"/"player entered the game" messages
		OldPC->PlayerState->Destroy();
		OldPC->PlayerState = NULL;
		}
		위의 코드를 보면 SeamlessTravelTo를 한 뒤에 Destroy를 하는 것을 볼 수 있다. 근데 Destroy의 과정에는 UnRegisterPlayerWithSession이 호출된다.
		즉, 마치 플레이어가 게임 세션을 나가게 된 것 처럼 된다.
		그럼 이후에 Register가 동작하냐면 그것 또한 아니다.
	*/
	// Super is not Working Acturally Because Engine Uses Defaults PlayerState so NAME_GameSession is used, not the SessionName
	// Super::UnregisterPlayerWithSession();
	
	if ((GetNetMode() == NM_Client && GetUniqueId().IsValid()) && !this->SeamlessTravling)
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

void AJudgePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	this->SeamlessTravling = true;
	PlayerState->OnRep_UniqueId();
}

void AJudgePlayerState::SetSessionName()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		SessionName = GameInstance->GetSessionName();
	}
}
