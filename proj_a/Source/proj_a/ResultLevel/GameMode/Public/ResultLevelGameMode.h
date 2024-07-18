#pragma once

#include "CoreMinimal.h"
#include "DPPlayerController.h"
#include "ScoreTypes.h"
#include "GameFramework/GameModeBase.h"
#include "ResultLevelGameMode.generated.h"

USTRUCT(BlueprintType)
struct FAnimalList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AnimalList")
	TArray<EAnimal> Animals;
};

USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PlayerName")
	FString PlayerName;
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerJob")
	EPlayerJob PlayerJob = EPlayerJob::JOB_ARCHAEOLOGIST;

	// index displays as player scores
	/*
	 * 0 : base score
	 * 1 : base score + alpha
	 * 2 : add mul score
	 * 3 : mul mul score
	 * 4 : total score
	 */
	UPROPERTY(BlueprintReadWrite, Category = "PlayerScore")
	TArray<int32> Scores;

	UPROPERTY(BlueprintReadWrite, Category = "CapturedAnimals")
	TArray<FAnimalList> CapturedAnimals;

	UPROPERTY(BlueprintReadWrite, Category = "bIsDetected")
	bool bIsDetected = false;
};

UCLASS()
class PROJ_A_API AResultLevelGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSimpleEvent);

	UPROPERTY(BlueprintAssignable, Category = "EventsResult")
	FOnSimpleEvent OnSimpleEvent;
	AResultLevelGameMode();

	virtual ~AResultLevelGameMode() override;

	/* Seamless Travel : Reuse PlayerController for Client */
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	/* Seamless Travel : Reuse PlayerController for Server */
	virtual void PostSeamlessTravel() override;

	virtual void BeginPlay() override;

	// index displays as player index
	UPROPERTY(BlueprintReadWrite, Category = "PlayerScores")
	TArray<FPlayerScore> PlayerScores;
	
protected:
	virtual void Logout(AController* Exiting) override;

private:
	TArray<int32> CalculateScores(ADPPlayerController* Controller);
	TArray<FAnimalList> GetCapturedAnimals(ADPPlayerController* Controller);
	void SetPlayerScores();
};
