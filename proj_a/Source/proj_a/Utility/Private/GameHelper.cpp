#include "GameHelper.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"

UWorld* UGameHelper::GetWorldFromContext(UObject* ContextObject)
{
	if (!ContextObject) return nullptr;
	return ContextObject->GetWorld();
}

AGameModeBase* UGameHelper::GetGameMode(UObject* ContextObject)
{
	if (UWorld* World = GetWorldFromContext(ContextObject))
	{
		return World->GetAuthGameMode<AGameModeBase>();
	}
	return nullptr;
}

AGameStateBase* UGameHelper::GetGameState(UObject* ContextObject)
{
	if (UWorld* World = GetWorldFromContext(ContextObject))
	{
		return World->GetGameState<AGameStateBase>();
	}
	return nullptr;
}
