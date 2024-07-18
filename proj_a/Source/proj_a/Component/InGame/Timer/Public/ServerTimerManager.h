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

	template<typename T, typename UserClass>
	void StartTimer(float Duration, void (UserClass::*Callback)(), UserClass* UserObject)
	{
		TimerDelegate = FTimerDelegate::CreateUObject(UserObject, Callback);
		check(TimerDelegate.IsBound());
		
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

protected:
	virtual void BeginPlay() override;

private:
	template<typename T>
	void TimerFunction()
	{
		if (TimeRemaining > 0)
		{
			TimeRemaining -= INTERVAL;
			TimeRemaining = FMath::Max(TimeRemaining, 0.0f);
			SetTimeRemaining<T>();
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			TimerDelegate.Execute();
		}
	}

	template<typename T>
	void SetTimeRemaining()
	{
		T* GS = GetWorld()->GetGameState<T>();
		check(GS)
		check(GS->TimerManager)
		GS->TimerManager->SetTimeRemaining(TimeRemaining);
		GS->TimerManager->OnRep_TimeRemaining();
	}
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	float TimeRemaining;
	float TimerDuration;
	bool bIsTimeOver = false;
};
