#include "JudgeGameState.h"

#include "DPPlayerController.h"
#include "JudgeLevelComponent.h"
#include "Net/UnrealNetwork.h"

AJudgeGameState::AJudgeGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
}

void AJudgeGameState::BeginPlay()
{
	Super::BeginPlay();

	ADPPlayerController* PC = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	checkf(PC, TEXT("Failed to get player controller"));
	PC->SetLevelComponent(NewObject<UJudgeLevelComp>(PC));
}

void AJudgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJudgeGameState, TimerManager);
}
