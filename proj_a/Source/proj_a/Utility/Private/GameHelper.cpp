#include "GameHelper.h"

#include "DPGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"

ADPGameModeBase* UGameHelper::GetInGameMode(UWorld* World)
{
	if (World)
	{
		if (AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			if (ADPGameModeBase* DPGameMode = Cast<ADPGameModeBase>(GameMode))
			{
				return DPGameMode;
			}
		}
	}
	return nullptr;
}

AGameStateBase* UGameHelper::GetInGameState(UObject* ContextObject)
{
	// if (UWorld* World = GetWorldFromContext(ContextObject))
	// {
	// 	return World->GetGameState<AGameStateBase>();
	// }
	return nullptr;
}
