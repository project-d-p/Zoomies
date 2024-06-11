#include "ServerTimerManager.h"
#include "TimerManager.h"
#include "DPInGameState.h"

UServerTimerManager::UServerTimerManager()
	: TimeRemaining(0), TimerDuration(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UServerTimerManager::BeginPlay()
{
	Super::BeginPlay();
}

#define INTERVAL 0.3f

void UServerTimerManager::StartTimer(float Duration)
{
	TimerDuration = Duration;
	TimeRemaining = TimerDuration;
	SetTimeRemaining();
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UServerTimerManager::TimerFunction,
		INTERVAL,
		true);
}

void UServerTimerManager::TimerFunction()
{
	if (TimeRemaining > 0)
	{
		TimeRemaining -= INTERVAL;
		
		SetTimeRemaining();
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Timer ended"));
	}
}

void UServerTimerManager::SetTimeRemaining()
{
	if (ADPInGameState* GS = GetWorld()->GetGameState<ADPInGameState>())
	{
		GS->TimerManager->SetTimeRemaining(TimeRemaining);
		GS->TimerManager->OnRep_TimeRemaining();
	}
}