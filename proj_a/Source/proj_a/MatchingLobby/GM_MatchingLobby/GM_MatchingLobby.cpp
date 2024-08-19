#include "GM_MatchingLobby.h"

#include "DPCharacter.h"
#include "isteamfriends.h"
#include "isteamuser.h"
#include "isteamutils.h"
#include "steamclientpublic.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "proj_a/MatchingLobby/PS_MatchingLobby/PS_MatchingLobby.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/GameInstance/GI_Zoomies.h"
#include "proj_a/MatchingLobby/SteamInvite/SteamInvite.h"

AGM_MatchingLobby::AGM_MatchingLobby() {
	GameStateClass = AGS_MatchingLobby::StaticClass();
	PlayerControllerClass = APC_MatchingLobby::StaticClass();
	PlayerStateClass = APS_MatchingLobby::StaticClass();
	DefaultPawnClass = nullptr; 
}

void AGM_MatchingLobby::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	//Set the host player index
	AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>();
	if (GS->HostPlayerIndex == -1)
	{
		GS->HostPlayerIndex = NewPlayer->PlayerState->GetPlayerId();
	}
	PCs.Add(NewPlayer);
	
	if (APC_MatchingLobby* PC = Cast<APC_MatchingLobby>(NewPlayer))
	{
		PC->SetCineCameraView();
	}
	CheckAndUpdateLobbyPlatform();
}

void AGM_MatchingLobby::Logout(AController* Exiting) {
	Super::Logout(Exiting);

	//Remove the player from the list of players
	if (APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting))
	{
		PCs.Remove(ExitingPlayer);
	}
	//Update the lobby platform
	CheckAndUpdateLobbyPlatform();
}

void AGM_MatchingLobby::BeginPlay() {
	Super::BeginPlay();
	FindAndStoreLobbyPlatforms();
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		GameInstance->LoadFriendsList();
	}
}

void AGM_MatchingLobby::CheckReadyToStart() 
{
	if (HasAuthority())
	{
		AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>();
		if (GS && GS->AreAllPlayersReady() && GetNumPlayers() >= MAX_USERS)
		{
			GS->MulticastShowLoadingWidget();
			StartGame_t();
		}
	}
}

void AGM_MatchingLobby::StartGame_t() const
{
	if (HasAuthority())
	{
		GetWorld()->ServerTravel(TEXT("mainLevel?listen") , true);
	}
}

void AGM_MatchingLobby::FindAndStoreLobbyPlatforms()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPlatform::StaticClass(), FoundActors);

	LobbyPlatforms.Init(nullptr, MAX_USERS);

	for (AActor* Actor : FoundActors)
	{
		if (Actor->Tags.Num() > 0)
		{
			FString Tag = Actor->Tags[0].ToString();
			int32 Index = FCString::Atoi(Tag.GetCharArray().GetData());
	
			if (Index >= 0 && Index < LobbyPlatforms.Num())
			{
				LobbyPlatforms[Index] = Cast<ALobbyPlatform>(Actor);
			}
		}
	}
	bIsLobbyPlatformReady = true;
}


void AGM_MatchingLobby::CheckAndUpdateLobbyPlatform()
{
	if (!bIsLobbyPlatformReady)
	{
		GetWorldTimerManager().SetTimer(
			UnusedHandle,
			this, &AGM_MatchingLobby::CheckAndUpdateLobbyPlatform,
			0.01f,
			false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(UnusedHandle);
		UpdatePlayerOnPlatform();
	}
}

void AGM_MatchingLobby::UpdatePlayerOnPlatform()
{
	for (int32 i = 0; i < PCs.Num(); i++)
	{
		bool bIsPlayerOnPlatform = false;
		
		//check player is already on platform
		if (LobbyPlatforms.IsValidIndex(i))
		{
			for (int32 j = 0; j < LobbyPlatforms.Num(); j++)
			{
				if (LobbyPlatforms[j] && LobbyPlatforms[j]->PC != nullptr)
				{
					if (PCs[i] == LobbyPlatforms[j]->PC)
					{
						bIsPlayerOnPlatform = true;
						break;
					}
				}
			}
		}
		
		//if player is not on platform, spawn character on platform
		if (!bIsPlayerOnPlatform)
		{
			//find the first available platform
			for (int32 j = 0; j < LobbyPlatforms.Num(); j++)
			{
				if (LobbyPlatforms[j] && LobbyPlatforms[j]->PC == nullptr)
				{
					LobbyPlatforms[j]->SpawnCharacter(PCs[i]);
					//get GameState and set PlayerController to the Lobby Infos
					if (AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>())
					{
						FString steam_username = PCs[i]->PlayerState->GetPlayerName();
						GS->LobbyInfos[j].Name = steam_username;
						GS->LobbyInfos[j].PC = Cast<APC_MatchingLobby>(PCs[i]);
						GS->LobbyInfos[j].PS = Cast<APS_MatchingLobby>(PCs[i]->PlayerState);
						GS->UpdateLobbyInfo();
					}
					break;
				}
			}
		}
	}

	for (int32 i = 0; i < LobbyPlatforms.Num(); i++)
	{
		if (LobbyPlatforms[i])
		{
			if (LobbyPlatforms[i]->PC && !PCs.Contains(LobbyPlatforms[i]->PC))
			{
				LobbyPlatforms[i]->Clear_Platform();
				if (AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>())
				{
					int32 playerIndex = i;
					if (playerIndex >= 0 && playerIndex < GS->ReadyPlayers.Num())
					{
						GS->ReadyPlayers[i] = false;
						GS->LobbyInfos[i].bIsReady = false;
					}
					else
					{
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Error: Player not found in ReadyPlayers"));
						}
					}
				}
			}
		}
	}
}