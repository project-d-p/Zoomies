#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerUI.h"
#include "TimerManager.h"
#include "ServerTimerManager.generated.h"

#define INTERVAL 0.3f

UCLASS()
class PROJ_A_API UServerTimerManager : public UActorComponent
{
	GENERATED_BODY()

public:    
	UServerTimerManager();

	template<typename T>
	void TimerFunction()
	{
		if (TimeRemaining > 0)
		{
			TimeRemaining -= INTERVAL;
			SetTimeRemaining<T>();
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			UE_LOG(LogTemp, Warning, TEXT("Timer ended"));
			bIsTimeOver = true;
		}
	}

	template<typename T>
	void SetTimeRemaining()
	{
		if (T* GS = GetWorld()->GetGameState<T>())
		{
			if (GS->GetTimerManager())
			{
				GS->GetTimerManager()->SetTimeRemaining(TimeRemaining);
				GS->GetTimerManager()->OnRep_TimeRemaining();
			}
		}
	}

	template<typename T>
	void StartTimer(float Duration)
	{
		bIsTimeOver = false;
		TimerDuration = Duration;
		TimeRemaining = TimerDuration;
		SetTimeRemaining<T>();
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&UServerTimerManager::TimerFunction<T>,
			INTERVAL,
			true);
	}
	
	bool IsTimeOver() const;

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle;
	float TimeRemaining;
	float TimerDuration;
	bool bIsTimeOver = false;
};
