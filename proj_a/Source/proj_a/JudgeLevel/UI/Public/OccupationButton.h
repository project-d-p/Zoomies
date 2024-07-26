#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "ScoreTypes.h"
#include "Components/Button.h"
#include "OccupationButton.generated.h"

UCLASS(BlueprintType, meta=(DisplayName="Occupation Button"))
class PROJ_A_API UOccupationButton : public UButton
{
	GENERATED_BODY()

public:
	UOccupationButton();
	void SetLambda(const std::function<void(EPlayerJob)>& InOnClickedCallback) {OnClickedCallback = InOnClickedCallback; }
	void SetOccupation(EPlayerJob InOccupation) { Occupation = InOccupation; }
private:
	EPlayerJob Occupation;
	std::function<void(EPlayerJob)> OnClickedCallback;
	UFUNCTION()
	void InternalOnClicked();
};