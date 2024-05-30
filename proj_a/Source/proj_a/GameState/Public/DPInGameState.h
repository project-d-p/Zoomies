#pragma once

#include "ScoreManagerComponent.h"
#include "GameFramework/GameStateBase.h"
#include "DPInGameState.generated.h"

UCLASS()
class PROJ_A_API ADPInGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ADPInGameState();
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UScoreManagerComponent* ScoreManagerComponent;
};
