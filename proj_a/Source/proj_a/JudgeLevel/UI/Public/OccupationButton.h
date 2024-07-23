#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "EnumTypes.h"
#include "OccupationButton.generated.h"

UCLASS(BlueprintType, meta=(DisplayName="Occupation Button"))
class PROJ_A_API UOccupationButton : public UButton
{
	GENERATED_BODY()

public:
	UOccupationButton();
	void SetLambda(const std::function<void(EOccupation)>& InOnClickedCallback) {OnClickedCallback = InOnClickedCallback; }
	void SetOccupation(EOccupation InOccupation) { Occupation = InOccupation; }
private:
	EOccupation Occupation;
	std::function<void(EOccupation)> OnClickedCallback;
	UFUNCTION()
	void InternalOnClicked();
};