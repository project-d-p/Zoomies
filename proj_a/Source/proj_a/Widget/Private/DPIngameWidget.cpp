// Fill out your copyright notice in the Description page of Project Settings.


#include "DPIngameWidget.h"

#include "Components/TextBlock.h"

void UDPIngameWidget::UpdateTimerDisplay(float TimeRemaining)
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
