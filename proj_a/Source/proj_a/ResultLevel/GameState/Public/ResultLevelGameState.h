#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ResultLevelGameState.generated.h"

UCLASS()
class AResultLevelGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AResultLevelGameState();
	
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void BeginPlay() override;
};
