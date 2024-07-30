// Fill out your copyright notice in the Description page of Project Settings.


#include "DPCalculateWidget.h"
#include "Components/HorizontalBox.h"

void UDPCalculateWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TArray<int32> UDPCalculateWidget::UpdateScores(const TArray<int32>& scores)
{
	TArray<int32> sortedIndices;
	for (int32 i = 0; i < scores.Num(); i++)
		sortedIndices.Add(i);

	sortedIndices.Sort([&](int32 a, int32 b) {
		return scores[a] > scores[b];
	});

	return sortedIndices;
}
