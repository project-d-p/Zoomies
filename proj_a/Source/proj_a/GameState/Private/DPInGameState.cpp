#include "DPInGameState.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "Net/UnrealNetwork.h"

ADPInGameState::ADPInGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ScoreManager = CreateDefaultSubobject<UClientScoreMananger>(TEXT("ScoreManager"));
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
}

void ADPInGameState::MulticastPlayerJob_Implementation() const
{
	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
		if (PlayerState)
		{
			switch (PlayerState->GetPlayerJob())
			{
			case EPlayerJob::JOB_ARCHAEOLOGIST:
				FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_ARCHAEOLOGIST"));
				break;
			case EPlayerJob::JOB_POACHER:
				FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_POACHER"));
				break;
			case EPlayerJob::JOB_RINGMASTER:
				FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_RINGMASTER"));
				break;
			case EPlayerJob::JOB_TERRORIST:
				FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_TERRORIST"));
				break;
			case EPlayerJob::JOB_ENVIRONMENTALIST:
				FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_ENVIRONMENTALIST"));
				break;
			default:
				break;
			}
		}
	}
}

void ADPInGameState::BeginPlay()
{
	Super::BeginPlay();

	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerController->SwitchLevelComponent(ELevelComponentType::MAIN);
	}
}

void ADPInGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void ADPInGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPInGameState, TimerManager);
	DOREPLIFETIME(ADPInGameState, ScoreManager);
}

