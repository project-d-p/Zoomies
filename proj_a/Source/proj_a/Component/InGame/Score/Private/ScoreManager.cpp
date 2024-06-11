#include "ScoreManager.h"

#include "DPPlayerController.h"
#include "PlayerScoreComp.h"

UScoreManagerComp::UScoreManagerComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UScoreManagerComp::BeginPlay()
{
	Super::BeginPlay();
}

void UScoreManagerComp::IncreasePlayerScore(APlayerController* PlayerController, int32 ScoreAmount)
{
	if (PlayerController == nullptr) return;

	ADPPlayerController* PC = Cast<ADPPlayerController>(PlayerController);
	if (PC == nullptr || PC->PlayerState == nullptr) return;
	
	UPlayerScoreComp* SM = PC->GetScoreManagerComponent();
	if (SM == nullptr) return;
	
	int32 CurrentScore = SM->PlayerScores;
	SM->PlayerScores = CurrentScore + ScoreAmount;

	SM->OnRep_PlayerScores();
}
