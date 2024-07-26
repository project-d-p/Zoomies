#pragma once

#include "CoreMinimal.h"
#include "ScoreTypes.h"
#include "GameFramework/GameState.h"
#include "ChatManager.h"
#include "DPCalculateWidget.h"
#include "DPResultWidget.h"
#include "IChatGameState.h"
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
class AResultLevelGameState : public AGameState, public IChatGameState
{
	GENERATED_BODY()
public:
	AResultLevelGameState();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayersAllTraveled();
	
	UFUNCTION(Client, Reliable)
	void NotifyPlayersAllTraveled();

	// index displays as player index
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerScores")
	TArray<FPlayerScore> PlayerScores;
	UPROPERTY(ReplicatedUsing=OnRep_IsAllSet)
	bool isAllSet = false;
	
	UPROPERTY(BlueprintReadOnly)
	int MyRank = 0;
	

	UChatManager* GetChatManager() const { return ChatManager; }
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_IsAllSet();
	
	void SetMyRank();
	void SetPlayerScores();
	

	TArray<int32> CalculateScores(/* ADPPlayerController* Controller */ TArray<TArray<EAnimal>> InCapturedAnimals, TArray<FScoreData> InScores);
	TArray<FAnimalList> GetCapturedAnimals(/*ADPPlayerController* Controller*/ TArray<TArray<EAnimal>> InCapturedAnimals);
	
	UPROPERTY()
	UChatManager* ChatManager;
	
	TSubclassOf<UDPCalculateWidget> CalculateWidget;
	TSubclassOf<UDPResultWidget> ResultWidget;
	TSubclassOf<AActor> BoardActor;

	UPROPERTY()
	UDPCalculateWidget* CalculateWidgetInstance;
	UPROPERTY()
	UDPResultWidget* ResultWidgetInstance;
};
