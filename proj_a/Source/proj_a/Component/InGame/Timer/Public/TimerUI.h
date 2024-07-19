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

	template<typename T>
	void initTimerUI(FTimerUiInitializer& Initializer)
	{
		if (Initializer.InWorld)
		{
			World = Initializer.InWorld;

			T* GS = Cast<T>(World->GetGameState());
			check(GS && GS->GetTimerManager())
			GS->GetTimerManager()->SetTimerUI(this);
		}
		if (Initializer.Time_Text)
		{
			Time_Text = Initializer.Time_Text;
		}
	}
private:
	UPROPERTY()
	UTextBlock* Time_Text = nullptr;

	UPROPERTY()
	UWorld* World = nullptr;
};
