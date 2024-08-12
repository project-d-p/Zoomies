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
	UFUNCTION()
	void InitScoreUi();
	void SetScoreUI(UScoreUI* InScoreUI);
	
private:
	UPROPERTY()
	UScoreUI* ScoreUI = nullptr;
	
};