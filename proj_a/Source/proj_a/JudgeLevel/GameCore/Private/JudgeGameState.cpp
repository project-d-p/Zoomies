#include "JudgeGameState.h"

#include "DPPlayerController.h"
#include "Net/UnrealNetwork.h"

AJudgeGameState::AJudgeGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
}

void AJudgeGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AJudgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJudgeGameState, TimerManager);
}
