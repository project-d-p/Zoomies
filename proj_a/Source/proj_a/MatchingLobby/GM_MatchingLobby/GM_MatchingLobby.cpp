#include "GM_MatchingLobby.h"

#include "DPCharacter.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "Kismet/GameplayStatics.h"

AGM_MatchingLobby::AGM_MatchingLobby() {
	// PlayerControllerClass = ADPPlayerController::StaticClass();
	GameStateClass = AGS_MatchingLobby::StaticClass();
	PlayerControllerClass = APC_MatchingLobby::StaticClass();
	DefaultPawnClass = ADPCharacter::StaticClass();

	static ConstructorHelpers::FClassFinder<AActor> ClassFinder(TEXT("Class'/Game/etc/Lobby/LobbyPlatform.LobbyPlatform_C'"));
	if (ClassFinder.Succeeded())
	{
		LobbyPlatformClass = ClassFinder.Class;
	}
}

void AGM_MatchingLobby::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>();

	if (GS->HostPlayerIndex == -1)
	{
		GS->HostPlayerIndex = NewPlayer->PlayerState->GetPlayerId();
	}
	PCs.Add(NewPlayer);
}

void AGM_MatchingLobby::Logout(AController* Exiting) {
	Super::Logout(Exiting);
	
	if (APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting))
	{
		PCs.Remove(ExitingPlayer);
	}
}

void AGM_MatchingLobby::BeginPlay() {
	Super::BeginPlay();
	FindAndStoreLobbyPlatforms();
	CheckLobbyReady();
}

void AGM_MatchingLobby::CheckReadyToStart() 
{
	if (HasAuthority())
	{
		AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>();
		if (GS && GS->AreAllPlayersReady() && GetNumPlayers() >= 2)
		{
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
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), LobbyPlatformClass, FoundActors);

	LobbyPlatforms.Init(nullptr, 4);
	FoundActors.Init(nullptr, 4);

	for (AActor* Actor : FoundActors)
	{
		if (Actor->Tags.Num() > 0)
		{
			// Get the first tag and convert it to a string, then to an integer
			FString Tag = Actor->Tags[0].ToString();
			int32 Index = FCString::Atoi(Tag.GetCharArray().GetData());

			// Check if the index is within the range of the LobbyPlatforms array
			if (Index >= 0 && Index < LobbyPlatforms.Num())
			{
				// Store the actor in the LobbyPlatforms array at the specified index
				LobbyPlatforms[Index] = Actor;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid index for LobbyPlatforms array"));
				return ;
			}
		}
	}
	bIsLobbyPlatformReady = true;
}

void AGM_MatchingLobby::CheckLobbyReady()
{
	if (!bIsLobbyPlatformReady)
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AGM_MatchingLobby::CheckLobbyReady, 0.01f, false);
	}
}