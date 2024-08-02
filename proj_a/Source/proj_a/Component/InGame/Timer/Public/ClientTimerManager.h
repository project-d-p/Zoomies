#pragma once

#include "CoreMinimal.h"
#include "TimerUI.h"
#include "Components/ActorComponent.h"
#include "ClientTimerManager.generated.h"

UCLASS()
class PROJ_A_API UClientTimerManager : public UActorComponent
{
	GENERATED_BODY()

public:   
	UClientTimerManager();
	
	void SetTimeRemaining(float NewTime);

	UPROPERTY(ReplicatedUsing = OnRep_TimeRemaining)
	float TimeRemaining;

	UFUNCTION()
	void OnRep_TimeRemaining();
	
	UFUNCTION() 
	void SetTimerUI(UTimerUI* inTime_Text);
protected:
	virtual void BeginPlay() override;
private:
	FTimerHandle TimerHandle;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	UTimerUI* Time_Text = nullptr;
};
