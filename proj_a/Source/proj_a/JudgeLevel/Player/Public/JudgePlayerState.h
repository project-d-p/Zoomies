#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ScoreTypes.h"
#include "JudgePlayerState.generated.h"

UCLASS()
class PROJ_A_API AJudgePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	void SetCapturedAnimals(const TArray<TArray<EAnimal>>& CapturedAnimals) { FinalScoreData.CapturedAnimals = CapturedAnimals; }
	void SetScoreDatas(const TArray<FScoreData>& ScoreDatas) { FinalScoreData.ScoreDatas = ScoreDatas; }
	void SetIsDetected(bool bIsDetected) { FinalScoreData.bIsDetected = bIsDetected; }
	FFinalScoreData GetFinalScoreData() const { return FinalScoreData; }

	void SetPlayerJob(EPlayerJob InPlayerJob) { PlayerJob = InPlayerJob; }
	EPlayerJob GetPlayerJob() const { return PlayerJob; }
	
private:
	FFinalScoreData FinalScoreData;
	EPlayerJob PlayerJob;
};
