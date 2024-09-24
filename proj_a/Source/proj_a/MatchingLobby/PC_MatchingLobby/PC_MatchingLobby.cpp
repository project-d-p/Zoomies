#include "PC_MatchingLobby.h"

#include "CineCameraActor.h"
#include "FNetLogger.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"
#include "Interfaces/OnlineSessionInterface.h"

// ServerSetReady: this Function called by Client but executed on Server
void APC_MatchingLobby::ServerSetReady_Implementation(bool pIsReady)
{
	AGS_MatchingLobby* GS_matching_lobby = GetWorld()->GetGameState<AGS_MatchingLobby>();
	AGM_MatchingLobby* GM_matching_lobby = GetWorld()->GetAuthGameMode<AGM_MatchingLobby>();
	
	if (GS_matching_lobby && GM_matching_lobby)
	{
		int32 PlayerIndex = GM_matching_lobby->PCs.Find(this);
		GS_matching_lobby->SetPlayerReady(PlayerIndex, pIsReady);
	}
}

bool APC_MatchingLobby::ServerSetReady_Validate(bool pIsReady)
{
	return true;
}

APC_MatchingLobby::APC_MatchingLobby()
{
	TextureTransferManager = CreateDefaultSubobject<UTextureTransferManager>(TEXT("TextureTransferManager"));
	TextureTransferManager->OnDataTransferComplete.BindDynamic(TextureTransferManager, &UTextureTransferManager::OnTextureTransferComplete);
}

void APC_MatchingLobby::ToggleReadyState()
{
	bIsReady = !bIsReady;
	ServerSetReady(bIsReady);
}

bool APC_MatchingLobby::GetIsReady()
{
	return bIsReady;
}

void APC_MatchingLobby::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	SteamInvite = NewObject<USteamInvite>();
	// SteamInvite->InitializeInviteCallback();
}

void APC_MatchingLobby::SetCineCameraView()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACineCameraActor::StaticClass(), FoundActors);

	ACineCameraActor* CineCamera = nullptr;
	for (AActor* Actor : FoundActors)
	{
		if (Actor->GetName() == "CineCameraActor_0")
		{
			CineCamera = Cast<ACineCameraActor>(Actor);
			break;
		}
	}

	if (!CineCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("CineCameraActor not found."));
		return;
	}

	this->SetViewTargetWithBlend(CineCamera);
}

void APC_MatchingLobby::ShowSteamInviteDialog()
{
	if (SteamInvite)
	{
		SteamInvite->ShowSteamOverlayInviteDialog();
	}
	else
	{
		//logging on screen about faile to show steam overlay with GEngine
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to show Steam Overlay"));
		}
	}
}
