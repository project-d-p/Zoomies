#include "ServerTimerManager.h"

#include "DPGameModeBase.h"
#include "TimerManager.h"
#include "DPInGameState.h"
#include "FNetLogger.h"
#include "JudgeGameState.h"

UServerTimerManager::UServerTimerManager()
	: TimeRemaining(0), TimerDuration(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UServerTimerManager::BeginPlay()
{
	Super::BeginPlay();
}


bool UServerTimerManager::IsTimeOver() const
{
	return bIsTimeOver;
}