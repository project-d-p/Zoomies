#pragma once

#include "ScoreManagerComponent.h"
#include "GameFramework/GameStateBase.h"
#include "ClientTimerManager.h"
#include "DPInGameState.generated.h"

UCLASS()
class PROJ_A_API ADPInGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ADPInGameState();

	UPROPERTY()
	UScoreManagerComponent* ScoreManagerComponent;

	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;

	UPROPERTY(ReplicatedUsing=OnRep_ServerTraveled)
	bool bServerTraveled;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ServerTraveled() const;
};
