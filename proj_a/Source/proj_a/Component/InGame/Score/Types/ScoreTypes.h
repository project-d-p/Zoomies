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

USTRUCT(BlueprintType)
struct FFinalScoreData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	FString PlayerName;

	//UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	TArray<TArray<EAnimal>> CapturedAnimals;

	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	TArray<FScoreData> ScoreDatas;

	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	float PrivateTotalScore = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	float PrivateTotalBaseScore = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	float PrivateTotalScale = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	float PublicTotalScore = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	bool bIsDetected = false;

	UPROPERTY(BlueprintReadWrite, Category = "Score Data")
	bool bIsLeft = false;

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
			PlayerName = InFinalScoreData.PlayerName;
			bIsDetected = InFinalScoreData.bIsDetected;
			bIsLeft = InFinalScoreData.bIsLeft;
			PrivateTotalScore = InFinalScoreData.PrivateTotalScore;
			PrivateTotalBaseScore = InFinalScoreData.PrivateTotalBaseScore;
			PrivateTotalScale = InFinalScoreData.PrivateTotalScale;
			PublicTotalScore = InFinalScoreData.PublicTotalScore;
		}
		return *this;
	}
};
