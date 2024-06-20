#include "PC_MatchingLobby.h"

#include "CineCameraActor.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
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