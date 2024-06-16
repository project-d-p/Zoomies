#include "ScoreManager.h"

#include "DPPlayerController.h"
#include "PlayerScoreComp.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"

UScoreManagerComp::UScoreManagerComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UScoreManagerComp::BeginPlay()
{
	Super::BeginPlay();
}

void UScoreManagerComp::IncreasePlayerScore(APlayerController* PlayerController, const TArray<EAnimal>& Animals)
{
	if (PlayerController == nullptr) return;

	ADPPlayerController* PC = Cast<ADPPlayerController>(PlayerController);
	if (PC == nullptr || PC->PlayerState == nullptr) return;
	
	UPlayerScoreComp* SM = PC->GetScoreManagerComponent();
	if (SM == nullptr) return;
	
	int32 CurrentScore = SM->PlayerScores;
	SM->PlayerScores = CurrentScore + Animals.Num();

	SM->OnRep_PlayerScores();
}
