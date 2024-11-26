#include "ResultLevelGameMode.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "IChatGameState.h"
#include "ResultLevelGameState.h"
#include "ServerChatManager.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AResultLevelGameMode::AResultLevelGameMode()
{
	bUseSeamlessTravel = true;

	PrimaryActorTick.bCanEverTick = true;
	
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ADPPlayerController::StaticClass();
	PlayerStateClass = ADPPlayerState::StaticClass();
	GameStateClass = AResultLevelGameState::StaticClass();

	ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
}

AResultLevelGameMode::~AResultLevelGameMode()
{
}

// Only Called On Server durign Host Migration
void AResultLevelGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SpawnNewPlayerPawn(NewPlayer);
	this->CurrentPlayerCount += 1;
	CheckPlayersAllTraveled();
}

void AResultLevelGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void AResultLevelGameMode::Logout(AController* Exiting)
{
	// Clear the session when the player leaves the game
	Super::Logout(Exiting);

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->AddBanPlayer(Exiting->PlayerState->GetUniqueId()->ToString());
	}
}

void AResultLevelGameMode::SpawnNewPlayerPawn(AController* PC)
{
	static int i = 0;
	if (i == 2)
	{
		i = 0;
	}
	
	FVector Location[2] = {
		{-527.514681,-128.409500,85.462503},
		{-527.514681,138.648437,85.462503}
	};

	FVector SpawnLocation = Location[i++];

	ADPCharacter* NewCharacter = GetWorld()->SpawnActor<ADPCharacter>(ADPCharacter::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
	if (NewCharacter)
	{
		NewCharacter->SetReplicatingMovement(true);
		PC->Possess(NewCharacter);
	}

	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(PC);
	if (PlayerController)
	{
		PlayerController->SwitchLevelComponent(ELevelComponentType::RESULT);
	}
}

void AResultLevelGameMode::SetPlayerNameTag(ADPPlayerController* Controller)
{
	ADPCharacter* Character = Cast<ADPCharacter>(Controller->GetCharacter());
	if (Character == nullptr)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, Controller]()
		{
			SetPlayerNameTag(Controller);
		}), 1.0f, false);
		return ;
	}
	Character->SetNameTag();
}

/* Seamless Travel : Reuse PlayerControllers */
void AResultLevelGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	SpawnNewPlayerPawn(C);
	this->CurrentPlayerCount += 1;
	CheckPlayersAllTraveled();
}

void AResultLevelGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AResultLevelGameMode::CheckPlayersAllTraveled()
{
	UGI_Zoomies* GI = Cast<UGI_Zoomies>(GetGameInstance());
	check(GI)

	int MaxPlayers = GI->network_failure_manager_->GetDesiredMaxPlayers() ? GI->network_failure_manager_->GetDesiredMaxPlayers() : GI->player_count;
	
	AResultLevelGameState* GS = Cast<AResultLevelGameState>(GetWorld()->GetGameState());
	check(GS)

	if (CurrentPlayerCount >= MaxPlayers)
	{
		FTimerHandle StartTimerHandle;
		FTimerDelegate StartTimerDelegate;

		StartTimerDelegate.BindLambda([this, GS]()
		{
			GS->NotifyPlayersAllTraveled();

			for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
			{
				ADPPlayerController* Controller = Cast<ADPPlayerController>(*it);
				if (Controller == nullptr)
				{
					continue;
				}
				SetPlayerNameTag(Controller);
			}
			
		});
		GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, StartTimerDelegate, 1.0f, false);
	}
}

void AResultLevelGameMode::BeginPlay()
{
	Super::BeginPlay();
}
