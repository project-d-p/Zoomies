#pragma once

#include "CoreMinimal.h"
#include "ScoreTypes.h"
#include "GameFramework/GameState.h"
#include "ChatManager.h"
#include "DPCalculateWidget.h"
#include "DPResultWidget.h"
#include "IChatGameState.h"
#include "ResultLevelGameState.generated.h"

UCLASS()
class AResultLevelGameState : public AGameState, public IChatGameState
{
	GENERATED_BODY()
public:
	AResultLevelGameState();

	void NotifyPlayersAllTraveled();

	UFUNCTION(BlueprintCallable)
	void SetWinner();
	UPROPERTY(BlueprintReadOnly)
	int MyRank = 0;

	UChatManager* GetChatManager() const { return ChatManager; }

	UFUNCTION(NetMulticast, Reliable)
	void NotifyAllScoresCalculated(const TArray<FFinalScoreData>& InFinalScoreDataArray);
	
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetMyRank();
	void SetPlayerScores();
	
	UPROPERTY()
	UChatManager* ChatManager;
	
	TSubclassOf<UDPCalculateWidget> CalculateWidget;
	TSubclassOf<UDPResultWidget> ResultWidget;
	TSubclassOf<AActor> BoardActor;

	UPROPERTY()
	TArray<FFinalScoreData> FinalScoreDataArray;

	UPROPERTY()
	UDPCalculateWidget* CalculateWidgetInstance;
	UPROPERTY()
	UDPResultWidget* ResultWidgetInstance;
};
