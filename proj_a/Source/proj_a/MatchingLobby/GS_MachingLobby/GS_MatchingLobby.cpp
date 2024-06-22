#include "GS_MatchingLobby.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"

AGS_MatchingLobby::AGS_MatchingLobby() {
	// Set Players Num. need to be Set
	ReadyPlayers.SetNum(MAX_USERS, false);
	BestHostPlayer = nullptr;
	LowestAveragePing = 202406071806.0f;
	HostPlayerIndex = -1;
}

void AGS_MatchingLobby::OnRep_ReadyPlayers()
{
	//ReadypPlayers array has been replicated
}

void AGS_MatchingLobby::SetPlayerReady(int32 PlayerIndex, bool bIsReady)
{
	int32 PlayerOrder = PlayerIndex - HostPlayerIndex;
	if (PlayerOrder >= 0 && PlayerOrder < ReadyPlayers.Num())
	{
		ReadyPlayers[PlayerOrder] = bIsReady;
	}
	// logging
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			30.f,
			FColor::Green,
			FString::Printf(TEXT("Player %d is %s"), PlayerIndex, bIsReady ? TEXT("Ready") : TEXT("Not Ready")));
	}
	if (HasAuthority())
	{
		AGM_MatchingLobby* GM = GetWorld()->GetAuthGameMode<AGM_MatchingLobby>();
		if (GM)
		{
			GM->CheckReadyToStart();
		}
	}
}

// set the ReadyPlayers array to be replicated
void AGS_MatchingLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGS_MatchingLobby, ReadyPlayers);
	DOREPLIFETIME(AGS_MatchingLobby, LowestAveragePing);
	DOREPLIFETIME(AGS_MatchingLobby, BestHostPlayer);
}

bool AGS_MatchingLobby::AreAllPlayersReady()
{
	for (int32 i = 0; i < ReadyPlayers.Num(); ++i)
	{
		if (!ReadyPlayers[i])
		{
			return false;
		}
	}
	FindFastestPlayer();
	return true;
}

//Find the fastest player
void AGS_MatchingLobby::FindFastestPlayer() 
{
	float average_ping = 0.0f;
	APlayerState* CurrentPlayerState = GetWorld()->GetFirstPlayerController()->PlayerState;
	
	AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
	if (GameState)
	{
		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			float PlayerPing = PlayerState->ExactPing;
			average_ping += PlayerPing;
		}
		average_ping /= GameState->PlayerArray.Num();
		
		//logging
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				30.f,
				FColor::Green,
				FString::Printf(TEXT("Average Ping: %f"), average_ping));
		}
		ReportPing(CurrentPlayerState, average_ping);
	}
}

void AGS_MatchingLobby::ReportPing_Implementation(APlayerState* ReportingPlayer, float AveragePing)
{
	if (AveragePing < LowestAveragePing)
	{
		LowestAveragePing = AveragePing;
		BestHostPlayer = ReportingPlayer;
	}
}

