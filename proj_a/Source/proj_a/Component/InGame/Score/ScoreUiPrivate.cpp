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
	PrivateScoreTextBlock_Front = Initializer.ScoreTextPrivate_Front;
	PrivateScoreTextBlock_Back = Initializer.ScoreTextPrivate_Back;
	PrivateScoreTextBlock_Job = Initializer.ScoreTextPrivate_Job;
	PrivateScoreTextBlock_Total = Initializer.ScoreTextPrivate_Total;
}

UTextBlock* UScoreUiPrivate::GetPrivateScoreTextBlock() const 
{
	return PrivateScoreTextBlock;
}
UTextBlock* UScoreUiPrivate::GetPrivateScoreTextBlock_Front() const
{
	return PrivateScoreTextBlock_Front;
}
UTextBlock* UScoreUiPrivate::GetPrivateScoreTextBlock_Back() const
{
	return PrivateScoreTextBlock_Back;
}
UTextBlock* UScoreUiPrivate::GetPrivateScoreTextBlock_Job() const
{
	return PrivateScoreTextBlock_Job;
}
UTextBlock* UScoreUiPrivate::GetPrivateScoreTextBlock_Total() const
{
	return PrivateScoreTextBlock_Total;
}

