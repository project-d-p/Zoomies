#pragma once

#include "ScoreManager.h"
#include "GameFramework/GameStateBase.h"
#include "ClientTimerManager.h"
#include "DPInGameState.generated.h"

UCLASS()
class PROJ_A_API ADPInGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ADPInGameState();

	UFUNCTION()
	void InitTimerManager(UTimerUI* TimerUI);
	
	UPROPERTY()
	UScoreManager* ScoreManagerComponent;

	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
