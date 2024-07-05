// Fill out your copyright notice in the Description page of Project Settings.


#include "DPResultRoomWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UDPResultRoomWidget::UpdateScores(const TArray<FString>& names, const TArray<int32>& scores)
{
    if (!scoresVerticalBox) 
        return;

    scoresVerticalBox->ClearChildren();

    TArray<int32> sortedIndices;
    for (int32 i = 0; i < scores.Num(); i++)
        sortedIndices.Add(i);

    sortedIndices.Sort([&](int32 a, int32 b) {
        return scores[a] > scores[b];
    });

    for (int32 i : sortedIndices) {
        UHorizontalBox* playerBox = NewObject<UHorizontalBox>(this);

        UTextBlock* nameText = NewObject<UTextBlock>(this);
        UTextBlock* scoreText = NewObject<UTextBlock>(this);

        if (nameText && scoreText && playerBox) {
            nameText->SetText(FText::FromString(names[i]));
            scoreText->SetText(FText::AsNumber(scores[i]));

            playerBox->AddChild(nameText);
            playerBox->AddChild(scoreText);

            scoresVerticalBox->AddChild(playerBox);
        }
    }
}
