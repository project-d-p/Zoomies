// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreUiPrivate.h"

#include "DPPlayerController.h"

void UScoreUiPrivate::InitScoreUiPrivate(const FPrivateScoreUiInitializer& Initializer)
{
	if (ADPPlayerController* PC = Cast<ADPPlayerController>(Initializer.InWorld->GetFirstPlayerController()))
	{
		PC->GetPrivateScoreManagerComponent()->SetScoreUI_Private(this);
	}
	PrivateScoreTextBlock = Initializer.ScoreTextPrivate;
}

UTextBlock* UScoreUiPrivate::GetPrivateScoreTextBlock() const
{
	return PrivateScoreTextBlock;
}

