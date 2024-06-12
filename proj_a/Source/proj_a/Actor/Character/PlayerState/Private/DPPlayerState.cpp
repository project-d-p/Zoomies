#include "DPPlayerState.h"

#include "FNetLogger.h"
#include "PlayerScoreComp.h"
#include "Net/UnrealNetwork.h"

ADPPlayerState::ADPPlayerState()
{
	// bReplicates = true;
	
	PlayerScoreComp = CreateDefaultSubobject<UPlayerScoreComp>(TEXT("PlayerScore"));
}

UPlayerScoreComp* ADPPlayerState::GetPlayerScoreComp() const
{
	return PlayerScoreComp;
}

void ADPPlayerState::BeginPlay()
{
	Super::BeginPlay();
}
//
// void ADPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	DOREPLIFETIME(ADPPlayerState, PlayerScoreComp);
// }
