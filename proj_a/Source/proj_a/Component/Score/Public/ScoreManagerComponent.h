// ScoreManagerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScoreManagerComponent.generated.h"

UCLASS()
class PROJ_A_API UScoreManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UScoreManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(Server, Reliable)
	void IncreasePlayer0Score(int32 Amount);
	UFUNCTION(Server, Reliable)
	void IncreasePlayer1Score(int32 Amount);
	UFUNCTION()
	int32 GetScore(int32 PlayerIndex) const;
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScoresUpdated)
	int32 Player0Scores;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScoresUpdated)
	int32 Player1Scores;

	UFUNCTION()
	void OnRep_PlayerScoresUpdated();
};