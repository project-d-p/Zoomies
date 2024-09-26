#include "PlayerScoreData.h"

#include "proj_a/Component/InGame/Score/CalculateScoreByJobs.h"

void UPlayerScoreData::InitializeData_Implementation()
{
	Super::InitializeData_Implementation();

	Score.CapturedAnimals.Empty();
	Score.ScoreDatas.Empty();
	Score.bIsDetected = false;
}

bool UPlayerScoreData::ValidateData_Implementation()
{
	bool ret = Super::ValidateData_Implementation();
	ret &= Score.CapturedAnimals.Num() >= 0;
	ret &= Score.ScoreDatas.Num() >= 0;
	ret &= Score.PrivateTotalScore >= 0;
	ret &= Score.PublicTotalScore >= 0;
	return ret;
}

void UPlayerScoreData::ResetData_Implementation()
{
	Super::ResetData_Implementation();

	Score.CapturedAnimals.Empty();
	Score.ScoreDatas.Empty();
	Score.bIsDetected = false;
}

UBaseData* UPlayerScoreData::Clone_Implementation()
{
	UPlayerScoreData* NewData = NewObject<UPlayerScoreData>(GetOuter(), GetClass());
	UBaseData::CloneData(NewData);	
	NewData->Score = Score;
	return NewData;
}

FName UPlayerScoreData::GetDataType() const
{
	return Super::GetDataType();
}

void UPlayerScoreData::IncreaseScore(const EPlayerJob& PlayerJob, const TArray<EAnimal>& Animals)
{
	int BaseAnimalScore = 100;
	FScoreData scoreData = UCalculateScoreByJobs::CalculateScoreByJobs(PlayerJob, Animals);
	Score.ScoreDatas.Add(scoreData);
	Score.CapturedAnimals.Add(Animals);
	Score.PublicTotalScore += Animals.Num() * BaseAnimalScore;
	Score.PrivateTotalBaseScore += scoreData.baseScore;
	Score.PrivateTotalScale += scoreData.addMulScore;
	Score.PrivateTotalScale *= scoreData.mulMulScore;
	Score.PrivateTotalScore = Score.PrivateTotalBaseScore * Score.PrivateTotalScale;
	OnDataChangedBroadcast();
}

void UPlayerScoreData::TestBroadcast()
{
	OnDataChangedBroadcast();
}
