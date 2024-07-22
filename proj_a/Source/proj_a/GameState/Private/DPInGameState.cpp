#include "DPInGameState.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "Net/UnrealNetwork.h"
#include "ELevelComponentType.h"

ADPInGameState::ADPInGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ScoreManager = CreateDefaultSubobject<UClientScoreMananger>(TEXT("ScoreManager"));
}

void ADPInGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	ScoreManager->InitScoreUi();
}

void ADPInGameState::BeginPlay()
{
	Super::BeginPlay();

	ScoreManager->InitScoreUi();
	// ADPCharacter* Character = Cast<ADPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	// if (Character)
	// {
	// 	Character->SetReplicatingMovement(false);
	// }
}

void ADPInGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPInGameState, TimerManager);
	DOREPLIFETIME(ADPInGameState, ScoreManager);
	DOREPLIFETIME(ADPInGameState, bServerTraveled);
}

void ADPInGameState::OnRep_ServerTraveled() const
{
	UE_LOG(LogTemp, Log, TEXT("Server traveled[CLIENT]"));
	FNetLogger::EditerLog(FColor::Green, TEXT("Server traveled[CLIENT]"));
	ADPPlayerController* my_controller = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());

	my_controller->SwitchLevelComponent(ELevelComponentType::MAIN);
	UClientSocket* my_socket = my_controller->GetClientSocket();
	my_socket->Connect("127.0.0.1", 4242);
	my_socket->RunTask();
}
