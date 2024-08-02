#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.h"
#include "ScoreTypes.generated.h"

struct FFinalScoreData
{
	TArray<TArray<EAnimal>> CapturedAnimals;
	TArray<FScoreData> ScoreDatas;
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
		}
		return *this;
	}
};

// UENUM(BlueprintType)
// enum class EPlayerJob : uint8
// {
// 	JOB_ARCHAEOLOGIST UMETA(DisplayName = "Archaeologist"),
// 	JOB_POACHER UMETA(DisplayName = "Poacher"),
// 	JOB_RINGMASTER UMETA(DisplayName = "Ringmaster"),
// 	JOB_TERRORIST UMETA(DisplayName = "Terrorist"),
// 	JOB_ENVIRONMENTALIST UMETA(DisplayName = "Environmentalist"),
// 	JOB_CHECK UMETA(DisplayName = "Check"),
// 	JOB_CROSS UMETA(DisplayName = "Cross"),
// };
//
// UENUM(BlueprintType)
// enum class EAnimal : uint8
// {
// 	ANIMAL_CRAB UMETA(DisplayName = "Crab"),
// 	ANIMAL_LOBSTER UMETA(DisplayName = "Lobster"),
// 	ANIMAL_MAMMOTH UMETA(DisplayName = "Mammoth"),
// 	ANIMAL_OCTOPUS UMETA(DisplayName = "Octopus"),
// 	ANIMAL_STAR_FISH UMETA(DisplayName = "StarFish"),
// 	ANIMAL_SLOTH UMETA(DisplayName = "Sloth"),
// 	ANIMAL_FOX UMETA(DisplayName = "Fox"),
// 	ANIMAL_GIRAFFE UMETA(DisplayName = "Giraffe"),
// 	ANIMAL_LION UMETA(DisplayName = "Lion"),
// 	// ANIMAL_RABBIT UMETA(DisplayName = "Rabbit"),
// 	ANIMAL_ELEPHANT UMETA(DisplayName = "Elephant"),
// 	ANIMAL_SABER_TOOTH_TIGER UMETA(DisplayName = "SaberToothTiger"),
// 	// ANIMAL_SHEPHERD UMETA(DisplayName = "Shepherd"),
// 	ANIMAL_SKUNK UMETA(DisplayName = "Skunk"),
// 	ANIMAL_SQUID UMETA(DisplayName = "Squid"),
// 	ANIMAL_DOLPHIN UMETA(DisplayName = "Dolphin"),
// 	ANIMAL_EEL UMETA(DisplayName = "Eel"),
// 	ANIMAL_PENGUIN UMETA(DisplayName = "Penguin"),
// 	ANIMAL_SEAL UMETA(DisplayName = "Seal"),
// 	ANIMAL_WHALE UMETA(DisplayName = "Whale"),
// 	ANIMAL_STINGRAY UMETA(DisplayName = "Stingray"),
// 	ANIMAL_HORSE UMETA(DisplayName = "Horse"),
// 	ANIMAL_CHAMELEON UMETA(DisplayName = "Chameleon"),
// };

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
