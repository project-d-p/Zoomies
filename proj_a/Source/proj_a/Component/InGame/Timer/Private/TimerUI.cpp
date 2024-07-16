#include "TimerUI.h"

#include "Components/TextBlock.h"

void UTimerUI::UpdateTimerDisplay(float TimeRemaining)
{
	if (Time_Text)
	{
		Time_Text->SetText(
			FText::FromString(
				FString::Printf(TEXT("%02d : %02d"),
					FMath::FloorToInt(TimeRemaining/60), FMath::FloorToInt(TimeRemaining)%60)
				)
			);
	}
}
