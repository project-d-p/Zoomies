// ScoreManagerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "ScoreUI.h"
#include "Components/ActorComponent.h"
#include "PlayerScoreComp.generated.h"

UCLASS()
class PROJ_A_API UPlayerScoreComp : public UActorComponent
{
	GENERATED_BODY()
public:
	UPlayerScoreComp();

	UFUNCTION()
	void OnRep_PlayerScores();

	UPROPERTY()
	UScoreUI* ScoreUI = nullptr;
	UPROPERTY()
	UTextBlock* PlayerScore_Text = nullptr;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScores)
	int32 PlayerScores = 0;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};