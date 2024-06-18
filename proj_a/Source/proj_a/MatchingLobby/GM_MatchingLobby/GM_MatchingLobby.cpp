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
	UpdatePlayerOnPlatform();
}

void AGM_MatchingLobby::Logout(AController* Exiting) {
	Super::Logout(Exiting);
	
	if (APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting))
	{
		PCs.Remove(ExitingPlayer);
	}
	UpdatePlayerOnPlatform();
}

void AGM_MatchingLobby::BeginPlay() {
	Super::BeginPlay();
	FindAndStoreLobbyPlatforms();
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

	for (AActor* Actor : FoundActors)
	{
		if (Actor->Tags.Num() > 0)
		{
			FString Tag = Actor->Tags[0].ToString();
			
			int32 Index = FCString::Atoi(Tag.GetCharArray().GetData());
	
			if (Index >= 0 && Index < LobbyPlatforms.Num())
			{
				LobbyPlatforms[Index] = Actor;
				if (APC_MatchingLobby* PCActor = Cast<APC_MatchingLobby>(Actor))
				{
					// PC 변수가 있는지 확인하고 출력
					if (PCActor->PC != nullptr)
						UE_LOG(LogTemp, Warning, TEXT("PC: %s"), PCActor->PC);
				}
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

void AGM_MatchingLobby::CheckLobbyPlatformReady()
{
	if (!bIsLobbyPlatformReady)
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle,
			this, &AGM_MatchingLobby::CheckLobbyPlatformReady,
			0.01f,
			false);
	}
	else
	{
		//log on screen platform ready
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Lobby Platforms are ready"));
		}
	}
}

void AGM_MatchingLobby::UpdatePlayerOnPlatform()
{
	CheckLobbyPlatformReady();
	for (int32 i = 0; i < PCs.Num(); i++)
	{
		bool bIsPlayerOnPlatform = false;
		if (LobbyPlatforms.IsValidIndex(i))
		{
			APC_MatchingLobby* PC = Cast<APC_MatchingLobby>(PCs[i]);
			if (PC)
			{
				//loop LobbyPlatforms
				for (int32 j = 0; j < LobbyPlatforms.Num(); j++)
				{
					APC_MatchingLobby * platformPC = Cast<APC_MatchingLobby>(LobbyPlatforms[j]);
					if (platformPC->PC == PC->PC)
					{
						bIsPlayerOnPlatform = true;
						break;
					}
					else
					{
						//log on screen about each PC value
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("PC: %s"), *PC->PC->GetName()));
							if (platformPC == nullptr)
							{
								GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("platformPC is Null")));
							}
						}
					}
				}
				if (!bIsPlayerOnPlatform)
				{
					//log on screed about player not on platform
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player not on platform"));
					}
					// for (int32 j = 0; j < LobbyPlatforms.Num();j++)
					// {
					// 	APC_MatchingLobby * platformPC = Cast<APC_MatchingLobby>(LobbyPlatforms[j]);
					// 	// if platformPC is not vaild
					// 	if (platformPC == nullptr)
					// 	{
					// 		platformPC->PC = PC->PC;
					// 		break;
					// 	}
					// }
				}
			}
		}
	}
}