#include "DPPlayerState.h"

#include "DPInGameState.h"
#include "FNetLogger.h"
#include "PlayerName.h"
#include "PlayerScoreComp.h"
#include "Net/UnrealNetwork.h"

ADPPlayerState::ADPPlayerState()
{
	// bReplicates = true;
	
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

void ADPPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ADPInGameState* GS = Cast<ADPInGameState>(GetWorld()->GetGameState());
	check(GS)
	GS->ScoreManager->InitScoreUi();
}

void ADPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(ADPPlayerState, PlayerScoreComp);
	// DOREPLIFETIME(ADPPlayerState, APlayerState::PlayerName);
	DOREPLIFETIME(ADPPlayerState, PlayerJob);
}
