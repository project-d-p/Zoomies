#include "JudgeGameState.h"

#include "DPPlayerController.h"
#include "JudgePlayerController.h"
#include "Net/UnrealNetwork.h"

AJudgeGameState::AJudgeGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
}

void AJudgeGameState::NotifyTimerEnd_Implementation()
{
	FNetLogger::EditerLog(FColor::Green, TEXT("NotifyTimerEnd"));
	AJudgePlayerController* PC = Cast<AJudgePlayerController>(GetWorld()->GetFirstPlayerController());
	check(PC && PC->GetJudgeLevelUI())
	FNetLogger::EditerLog(FColor::Green, TEXT("JudgeLevelUI: %d"), PC->GetJudgeLevelUI()->GetVote());
	PC->ReturnVote(PC->GetJudgeLevelUI()->GetVote());
}

void AJudgeGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AJudgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJudgeGameState, TimerManager);
}
