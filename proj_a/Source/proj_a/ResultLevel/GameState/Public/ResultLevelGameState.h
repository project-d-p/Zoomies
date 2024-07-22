#pragma once

#include "CoreMinimal.h"
#include "ScoreTypes.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameStateBase.h"
#include "ResultLevelGameState.generated.h"

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

	UPROPERTY(BlueprintReadWrite, Category = "Rank")
	int Rank = 0;
};

class ADPPlayerController;

UCLASS()
class AResultLevelGameState : public AGameState
{
	GENERATED_BODY()
public:
	AResultLevelGameState();

	// index displays as player index
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerScores")
	TArray<FPlayerScore> PlayerScores;
	
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	TArray<int32> CalculateScores(ADPPlayerController* Controller);
	TArray<FAnimalList> GetCapturedAnimals(ADPPlayerController* Controller);
	
	void SetMyRank();
	void SetPlayerScores();
};
