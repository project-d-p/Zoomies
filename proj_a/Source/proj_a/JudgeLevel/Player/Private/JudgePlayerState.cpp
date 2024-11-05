#include "JudgePlayerState.h"

#include "proj_a/GameInstance/GI_Zoomies.h"

void AJudgePlayerState::BeginPlay()
{
	Super::BeginPlay();

	SetSessionName();
}

void AJudgePlayerState::SetSessionName()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		SessionName = GameInstance->GetSessionName();
	}
}
