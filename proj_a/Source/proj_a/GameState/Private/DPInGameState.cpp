#include "DPInGameState.h"

#include "DPPlayerController.h"
#include "Net/UnrealNetwork.h"

ADPInGameState::ADPInGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ScoreManager = CreateDefaultSubobject<UClientScoreMananger>(TEXT("ScoreManager"));
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
}

void ADPInGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void ADPInGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPInGameState, TimerManager);
	DOREPLIFETIME(ADPInGameState, ScoreManager);
}
