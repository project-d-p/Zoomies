#pragma once

#include "ClientTimerManager.h"
#include "GameFramework/GameStateBase.h"
#include "JudgeGameState.generated.h"

UCLASS()
class PROJ_A_API AJudgeGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AJudgeGameState();

	UPROPERTY(Replicated)
	UClientTimerManager* TimerManager;
};
