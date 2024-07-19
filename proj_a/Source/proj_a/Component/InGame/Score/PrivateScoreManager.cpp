#include "PrivateScoreManager.h"
#include "CalculateScoreByJobs.h"
#include "FNetLogger.h"
#include "ScoreUiPrivate.h"

UPrivateScoreManager::UPrivateScoreManager()
{
	FNetLogger::LogInfo(TEXT("UPrivateScoreManager::UPrivateScoreManager"));
}

void UPrivateScoreManager::BeginPlay()
{
	Super::BeginPlay();
}

void UPrivateScoreManager::IncreasePrivatePlayerScore(const EPlayerJob& playerJob, const TArray<EAnimal>& animals)
{
	FScoreData newScore = UCalculateScoreByJobs::CalculateScoreByJobs(playerJob, animals);
	PrivatePlayerBaseScore += newScore.baseScore; 
	PrivatePlayerMulScore += newScore.addMulScore;
	PrivatePlayerMulScore *= newScore.mulMulScore;
	PrivatePlayerScore = PrivatePlayerBaseScore * PrivatePlayerMulScore;
	
	CapturedAnimals.Add(animals);
	ScoreDatas.Add(newScore);
	UpdatePrivatePlayerScoreUI();
}

void UPrivateScoreManager::IncreasePrivatePlayerScoreByServer(const EPlayerJob& playerJob, const TArray<EAnimal>& animals)
{
	FScoreData newScore = UCalculateScoreByJobs::CalculateScoreByJobs(playerJob, animals);
	PrivatePlayerBaseScore += newScore.baseScore; 
	PrivatePlayerMulScore += newScore.addMulScore;
	PrivatePlayerMulScore *= newScore.mulMulScore;
	PrivatePlayerScore = PrivatePlayerBaseScore * PrivatePlayerMulScore;
	
	CapturedAnimals.Add(animals);
	ScoreDatas.Add(newScore);
}

void UPrivateScoreManager::UpdatePrivatePlayerScoreUI()
{
	if (ScoreTextPrivate == nullptr)
	{
		ScoreTextPrivate = ScoreUI_Private->GetPrivateScoreTextBlock();
	}	
	if (ScoreTextPrivate != nullptr)
	{
		ScoreTextPrivate->SetText(FText::FromString(FString::FromInt(PrivatePlayerScore)));
	}
}

void UPrivateScoreManager::SetScoreUI_Private(UScoreUiPrivate* pScoreUI_Private)
{
	if (pScoreUI_Private == nullptr)
	{
		return	;
	}
	ScoreUI_Private = pScoreUI_Private;
}
