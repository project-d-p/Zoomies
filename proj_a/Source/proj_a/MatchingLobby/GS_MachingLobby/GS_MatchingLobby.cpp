#include "GS_MatchingLobby.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

AGS_MatchingLobby::AGS_MatchingLobby() {
	ReadyPlayers.SetNum(2, false);
	HostPlayerIndex = -1;
}

// when the ReadyPlayers array is replicated, this function is called
void AGS_MatchingLobby::OnRep_ReadyPlayers()
{
	//need Ready UI changes logic
}

void AGS_MatchingLobby::SetPlayerReady(int32 PlayerIndex, bool bIsReady)
{
	int32 PlayerOrder = PlayerIndex - HostPlayerIndex;
	//playerIndex Logging
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			30.f,
			FColor::Green,
			FString::Printf(TEXT("PlayerIndex: %d try to Ready"), PlayerIndex));
		GEngine->AddOnScreenDebugMessage(
			-1,
			30.f,
			FColor::Green,
			FString::Printf(TEXT("PlayerOrder: %d try to Ready"), PlayerOrder));
	}
	if (PlayerOrder >= 0 && PlayerOrder < ReadyPlayers.Num())
	{
		ReadyPlayers[PlayerOrder] = bIsReady;
		
		//logging
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				30.f,
				FColor::Green,
				FString::Printf(TEXT("Player %d is ready"), PlayerIndex));
		}
		OnRep_ReadyPlayers();
	}	
}

// set the ReadyPlayers array to be replicated
void AGS_MatchingLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGS_MatchingLobby, ReadyPlayers);
}

bool AGS_MatchingLobby::AreAllPlayersReady() const
{
	for (int32 i = 0; i < ReadyPlayers.Num(); ++i)
	{
		if (!ReadyPlayers[i])
		{
			//logging
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					30.f,
					FColor::Green,
					FString::Printf(TEXT("Player %d is not ready"), i + HostPlayerIndex));
			}
			return false;
		}
	}
	FindFastestPlayer();
	//logging
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			30.f,
			FColor::Green,
			FString::Printf(TEXT("All players are ready")));
	}
	// GetWorld()->ServerTravel(TEXT("mainLevel?listen"), true);
	return true;
}

APlayerState* AGS_MatchingLobby::FindFastestPlayer() const
{
	APlayerState* FastestPlayer = nullptr;
	float LowestPing = 202406071305;

	for (APlayerState* PlayerState : GetWorld()->GetGameState()->PlayerArray)
	{
		float PlayerPing = PlayerState->ExactPing;

		if (PlayerPing < LowestPing)
		{
			LowestPing = PlayerPing;
			FastestPlayer = PlayerState;
		}
	}
	//logging
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			30.f,
			FColor::Green,
			FString::Printf(TEXT("Fastest Player: %s"), *FastestPlayer->GetPlayerName()));
	}
	return FastestPlayer;
}