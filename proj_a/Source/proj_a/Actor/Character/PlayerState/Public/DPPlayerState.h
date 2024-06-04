#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ScoreManager.h"
#include "DPPlayerState.generated.h"

UCLASS()
class PROJ_A_API ADPPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADPPlayerState();
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScoresUpdated)
	UScoreManager* ScoreManager = nullptr;

	UFUNCTION()
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
