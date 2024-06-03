#include "TimerUI.h"

#include "DPInGameState.h"
#include "DPPlayerController.h"
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

void UTimerUI::initTimerUI(FTimerUiInitializer& Initializer)
{
	if (Initializer.InWorld)
	{
		World = Initializer.InWorld;

		ADPInGameState *GS = Cast<ADPInGameState>(World->GetGameState());
		if (GS)
		{
			GS->TimerManager->SetTimerUI(this);
		}
	}
	if (Initializer.Time_Text)
	{
		Time_Text = Initializer.Time_Text;
	}
}
