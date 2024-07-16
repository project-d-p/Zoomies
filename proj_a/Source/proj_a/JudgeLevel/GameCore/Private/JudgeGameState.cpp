#include "JudgeGameState.h"

#include "Net/UnrealNetwork.h"

AJudgeGameState::AJudgeGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
}

void AJudgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJudgeGameState, TimerManager);
}
