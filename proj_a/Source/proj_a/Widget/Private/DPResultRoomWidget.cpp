// Fill out your copyright notice in the Description page of Project Settings.


#include "DPResultRoomWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

TArray<int32> UDPResultRoomWidget::UpdateScores(const TArray<int32>& scores)
{
	TArray<int32> sortedIndices;
	for (int32 i = 0; i < scores.Num(); i++)
		sortedIndices.Add(i);

	sortedIndices.Sort([&](int32 a, int32 b) {
		return scores[a] > scores[b];
	});

	return sortedIndices;
}
