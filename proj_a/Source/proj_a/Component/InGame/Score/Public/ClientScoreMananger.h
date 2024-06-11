#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScoreUI.h"
#include "ClientScoreMananger.generated.h"

UCLASS()
class UClientScoreMananger : public UActorComponent
{
	GENERATED_BODY()
public:
	UClientScoreMananger();
	
	UFUNCTION()
	void InitScoreUi();
private:
	UPROPERTY(Replicated)
	UScoreUI* ScoreUI = nullptr;
public:
	/** Setter */
	void SetScoreUI(UScoreUI* InScoreUI);
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};