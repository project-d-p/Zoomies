#include "JudgePlayerState.h"

#include "proj_a/GameInstance/GI_Zoomies.h"

AJudgePlayerState::AJudgePlayerState()
{
	// SetSessionName();
}

void AJudgePlayerState::OnRep_UniqueId()
{
	SetSessionName();
	
	Super::OnRep_UniqueId();
}

void AJudgePlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AJudgePlayerState::SetSessionName()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		SessionName = GameInstance->GetSessionName();
	}
}
