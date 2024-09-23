#include "JudgeGameState.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "JudgeLevelComponent.h"
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
	AJudgePlayerController* PC = Cast<AJudgePlayerController>(GetWorld()->GetFirstPlayerController());
	// ADPPlayerController* PC = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->GetJudgeLevelUI())
		return ;
	// UJudgeLevelComponent* JLC = Cast<UJudgeLevelComponent>(PC->GetLevelComponent());
	// if (!JLC || !JLC->GetJudgeLevelUI())
	// 	return;
	// JLC->ReturnVote(JLC->GetJudgeLevelUI()->GetVote());
	PC->ReturnVote(PC->GetJudgeLevelUI()->GetVote());
}

void AJudgeGameState::SetVoterName_Implementation(const FString& Name)
{
	AJudgePlayerController* PC = Cast<AJudgePlayerController>(GetWorld()->GetFirstPlayerController());
	// ADPPlayerController* PC = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());

	// XXX: This code may fail if the client's loading is slow.
	// In such cases, the voter name might not be set properly.
	// TODO: Address this after alpha testing.
	check(PC)
	// UJudgeLevelComponent* JLC = Cast<UJudgeLevelComponent>(PC->GetLevelComponent());
	// check(JLC)
	PC->GetJudgeLevelUI()->SetVoterName(Name);
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
