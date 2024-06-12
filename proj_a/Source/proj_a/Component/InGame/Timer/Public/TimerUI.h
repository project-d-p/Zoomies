#pragma once
#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "TimerUI.generated.h"

USTRUCT()
struct FTimerUiInitializer
{
	GENERATED_BODY();

	UPROPERTY()
	UWorld* InWorld;
	UPROPERTY()
	UTextBlock* Time_Text;
};

UCLASS()
class UTimerUI : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void UpdateTimerDisplay(float TimeRemaining);
	UFUNCTION()
	void initTimerUI(FTimerUiInitializer& Initializer);
private:
	UPROPERTY()
	UTextBlock* Time_Text = nullptr;

	UPROPERTY()
	UWorld* World = nullptr;
};
