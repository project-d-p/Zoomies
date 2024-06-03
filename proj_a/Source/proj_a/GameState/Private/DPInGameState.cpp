#include "DPInGameState.h"

#include "FNetLogger.h"
#include "Net/UnrealNetwork.h"

ADPInGameState::ADPInGameState()
{
	bReplicates = true;
	
	ScoreManagerComponent = CreateDefaultSubobject<UScoreManagerComponent>(TEXT("ScoreManagerComponent"));
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
}

void ADPInGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPInGameState, TimerManager);
	// DOREPLIFETIME(ADPInGameState, ScoreManagerComponent);
}