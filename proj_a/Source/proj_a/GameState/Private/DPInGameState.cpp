#include "DPInGameState.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "Net/UnrealNetwork.h"

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
	// Client¡Æ¢® ¨ù??? ¢¯?¡Æ? ¡Æ¢®¢¥??
	// FSocketManager::GetInstance().Connect("127.0.0.1", 4242);
	// FSocketManager::GetInstance().RunTask();
	ADPPlayerController* my_controller = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	my_controller->CreateSocket();
	my_controller->Connect("127.0.0.1", 4242);
	my_controller->RunTask();
}
