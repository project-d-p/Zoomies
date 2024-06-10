#include "DPInGameState.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "Net/UnrealNetwork.h"

ADPInGameState::ADPInGameState()
{
	bReplicates = true;
	bServerTraveled = false;
	ScoreManagerComponent = CreateDefaultSubobject<UScoreManagerComponent>(TEXT("ScoreManagerComponent"));
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
}

void ADPInGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPInGameState, TimerManager);
	// DOREPLIFETIME(ADPInGameState, ScoreManagerComponent);
	DOREPLIFETIME(ADPInGameState, bServerTraveled);
}

void ADPInGameState::OnRep_ServerTraveled() const
{
	UE_LOG(LogTemp, Log, TEXT("Server traveled[CLIENT]"));
	FNetLogger::EditerLog(FColor::Green, TEXT("Server traveled[CLIENT]"));
	// Client가 소켓 연결 가능?
	// FSocketManager::GetInstance().Connect("127.0.0.1", 4242);
	// FSocketManager::GetInstance().RunTask();
	ADPPlayerController* my_controller = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	my_controller->CreateSocket();
	my_controller->Connect("127.0.0.1", 4242);
	my_controller->RunTask();
}

