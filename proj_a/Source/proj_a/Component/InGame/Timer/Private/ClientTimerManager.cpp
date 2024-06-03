#include "ClientTimerManager.h"

#include "FNetLogger.h"
#include "Net/UnrealNetwork.h"

UClientTimerManager::UClientTimerManager()
{
	SetIsReplicatedByDefault(true);
	// PrimaryComponentTick.bCanEverTick = true;

	TimeRemaining = 0.0f;
}

void UClientTimerManager::SetTimerUI(UTimerUI* inTime_Text)
{
	Time_Text = inTime_Text;
}

void UClientTimerManager::BeginPlay()
{
	Super::BeginPlay();
}

void UClientTimerManager::SetTimeRemaining(float NewTime)
{
	TimeRemaining = NewTime;
}

void UClientTimerManager::OnRep_TimeRemaining()
{
	if (Time_Text)
	{
		Time_Text->UpdateTimerDisplay(TimeRemaining);
	}
}

void UClientTimerManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UClientTimerManager, TimeRemaining);
}