#pragma once

#include "CoreMinimal.h"
#include "PlayerScoreData.h"
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
	void SetIsDetected(bool bIsDetected) { if(PlayerScoreData) {PlayerScoreData->SetIsDetected(bIsDetected);} }
	FFinalScoreData GetFinalScoreData() const { return FinalScoreData; }
	UPlayerScoreData* GetPlayerScoreData() const { return PlayerScoreData; }

	void SetPlayerJob(EPlayerJob InPlayerJob) { PlayerJob = InPlayerJob; }
	EPlayerJob GetPlayerJob() const { return PlayerJob; }
	void SetPlayerScoreData(UPlayerScoreData* InPlayerScoreData) { PlayerScoreData = Cast<UPlayerScoreData>(InPlayerScoreData->Clone(this)); }
	
private:
	UPROPERTY()
	UPlayerScoreData* PlayerScoreData;
	FFinalScoreData FinalScoreData;
	EPlayerJob PlayerJob;
};
