#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.h"
#include "ScoreTypes.generated.h"

USTRUCT(BlueprintType)
struct FScoreData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	int baseScore = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	float addMulScore = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score")
	float mulMulScore = 1;

};

struct FFinalScoreData
{
	FString PlayerName;
	TArray<TArray<EAnimal>> CapturedAnimals;
	TArray<FScoreData> ScoreDatas;
	float PrivateTotalScore = 0;
	float PrivateTotalBaseScore = 0;
	float PrivateTotalScale = 1;
	float PublicTotalScore = 0;
	bool bIsDetected = false;

	FFinalScoreData& operator=(const FFinalScoreData& InFinalScoreData)
	{
		if (this != &InFinalScoreData)
		{
			CapturedAnimals.Empty();
			for (const auto& AnimalArray : InFinalScoreData.CapturedAnimals)
				CapturedAnimals.Add(AnimalArray);
			ScoreDatas.Empty();
			for (const auto& ScoreData : InFinalScoreData.ScoreDatas)
				ScoreDatas.Add(ScoreData);
			bIsDetected = InFinalScoreData.bIsDetected;
			PrivateTotalScore = InFinalScoreData.PrivateTotalScore;
			PrivateTotalBaseScore = InFinalScoreData.PrivateTotalBaseScore;
			PrivateTotalScale = InFinalScoreData.PrivateTotalScale;
			PublicTotalScore = InFinalScoreData.PublicTotalScore;
		}
		return *this;
	}
};
