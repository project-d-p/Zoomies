#include "PC_MatchingLobby.h"

#include "CineCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"
#include "proj_a/MatchingLobby/A_MatchingLobby/LC_MatchLobby.h"

APC_MatchingLobby::APC_MatchingLobby()
{
	bIsReady = false;
}

void APC_MatchingLobby::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	ADPCharacter* DPCharacter = Cast<ADPCharacter>(P);
	DPCharacter->SetReplicatingMovement(true);
}

void APC_MatchingLobby::ServerSetReady_Implementation(bool pIsReady)
{
	AGS_MatchingLobby* GS_MatchLobby= GetWorld()->GetGameState<AGS_MatchingLobby>();
	AGM_MatchingLobby* GM_MatchLobby = GetWorld()->GetAuthGameMode<AGM_MatchingLobby>();
	
	if (GM_MatchLobby && GS_MatchLobby)
	{
		int32 PlayerIndex = GM_MatchLobby->PCs.Find(this);
		GS_MatchLobby->SetPlayerReady(PlayerIndex, pIsReady);
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
	ActivateCurrentComponent(this);
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

void APC_MatchingLobby::ActivateCurrentComponent(APC_MatchingLobby* LocalPlayerController)
{
	LevelComponent = CreateDefaultSubobject<ULC_MatchLobby>(TEXT("ULC_MatchLobby"));
	if (LevelComponent)
	{
		LevelComponent->PrimaryComponentTick.bCanEverTick = true;
		LevelComponent->Activate(true);
		LevelComponent->SetComponentTickEnabled(true);
		LevelComponent->RegisterComponent();

		if (LocalPlayerController)
		{
			LevelComponent->Set_PC(LocalPlayerController);
			if (!LocalPlayerController->IsLocalController())
			{
				UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LocalPlayerController is not local controller"));
				return ;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LocalPlayerController is nullptr"));
			return ;
		}

		if (UIC_MatchLobby* IC_Local = LevelComponent->GetInputComponent())
		{
			IC_Local->Activate(true);
		}
	}
}

void APC_MatchingLobby::DeactiveCurrentComponent()
{
	if (LevelComponent)
	{
		if (UIC_MatchLobby* IC_Local = LevelComponent->GetInputComponent())
		{
			IC_Local->Deactivate();
		}
		
		LevelComponent->Deactivate();
		LevelComponent->SetComponentTickEnabled(false);
		LevelComponent->PrimaryComponentTick.bCanEverTick = false;
		LevelComponent->RegisterComponent();
		LevelComponent = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LevelComponent is nullptr"));
	}
}

void APC_MatchingLobby::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	LevelComponent->Set_CHAR(InPawn);
	LevelComponent->GetInputComponent()->Set_CHAR(InPawn);
}