#pragma once

#include "GameFramework/GameStateBase.h"
#include "ClientTimerManager.h"
#include "ClientScoreMananger.h"
#include "DPInGameState.generated.h"

UCLASS()
class PROJ_A_API ADPInGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ADPInGameState();

	// UFUNCTION()
	// void InitTimerManager(UTimerUI* TimerUI);

	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;
	UPROPERTY(Replicated)
	UClientScoreMananger* ScoreManager;
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
