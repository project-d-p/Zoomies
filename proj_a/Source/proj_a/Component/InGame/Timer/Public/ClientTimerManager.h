#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClientTimerManager.generated.h"

UCLASS()
class PROJ_A_API UClientTimerManager : public UActorComponent
{
	GENERATED_BODY()

public:   
	UClientTimerManager();
	
	void SetTimeRemaining(float NewTime);
	void Update();

	UPROPERTY(ReplicatedUsing = OnRep_TimeRemaining)
	// UPROPERTY()
	float TimeRemaining;

protected:
	virtual void BeginPlay() override;
private:
	FTimerHandle TimerHandle;
	UFUNCTION()
	void OnRep_TimeRemaining();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
