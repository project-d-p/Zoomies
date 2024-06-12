#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerUI.h"
#include "ServerTimerManager.generated.h"

UCLASS()
class PROJ_A_API UServerTimerManager : public UActorComponent
{
	GENERATED_BODY()

public:    
	UServerTimerManager();

	UFUNCTION()
	void TimerFunction();
	void SetTimeRemaining();

	void StartTimer(float Duration);

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle;
	float TimeRemaining;
	float TimerDuration;
};
