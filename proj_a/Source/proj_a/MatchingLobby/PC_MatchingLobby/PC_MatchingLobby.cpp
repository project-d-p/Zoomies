#include "PC_MatchingLobby.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"

// ServerSetReady: this Function called by Client but executed on Server
void APC_MatchingLobby::ServerSetReady_Implementation(bool pIsReady)
{
	AGS_MatchingLobby* GS_matching_lobby = GetWorld()->GetGameState<AGS_MatchingLobby>();
	if (GS_matching_lobby)
	{
		int32 PlayerIndex = PlayerState->GetPlayerId();
		GS_matching_lobby->SetPlayerReady(PlayerIndex, pIsReady);
	}
}

bool APC_MatchingLobby::ServerSetReady_Validate(bool pIsReady)
{
	return true;
}

void APC_MatchingLobby::ToggleReadyState()
{
	bIsReady = !bIsReady;
	ServerSetReady(bIsReady);
}

void APC_MatchingLobby::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}