#include "DPPlayerState.h"

#include "FNetLogger.h"
#include "PlayerName.h"
#include "PlayerScoreComp.h"
#include "Net/UnrealNetwork.h"

ADPPlayerState::ADPPlayerState()
{
	bReplicates = true;
	
	PlayerScoreComp = CreateDefaultSubobject<UPlayerScoreComp>(TEXT("PlayerScore"));
	FString PlayerName = FGuid::NewGuid().ToString();
	APlayerState::SetPlayerName(PlayerName);
}

UPlayerScoreComp* ADPPlayerState::GetPlayerScoreComp() const
{
	return PlayerScoreComp;
}

EPlayerJob ADPPlayerState::GetPlayerJob() const
{
	return PlayerJob;
}

void ADPPlayerState::OnRep_Rank()
{
}

void ADPPlayerState::ServerSetRank_Implementation(int InRank)
{
	Rank = InRank;

	OnRep_Rank();
}

void ADPPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ADPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPPlayerState, PlayerJob);
	DOREPLIFETIME(ADPPlayerState, Rank);
}
