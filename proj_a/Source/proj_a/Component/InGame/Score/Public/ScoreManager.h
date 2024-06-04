// ScoreManagerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScoreManager.generated.h"

UCLASS()
class PROJ_A_API UScoreManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UScoreManager();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(Server, Reliable)
	void IncreasePlayerScore(int32 Amount);
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScoresUpdated)
	int32 PlayerScores = 0;

	UFUNCTION()
	void OnRep_PlayerScoresUpdated();
};