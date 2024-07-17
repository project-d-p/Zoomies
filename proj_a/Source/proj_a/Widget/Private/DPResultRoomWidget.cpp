// Fill out your copyright notice in the Description page of Project Settings.


#include "DPResultRoomWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

void UDPResultRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//static ConstructorHelpers::FObjectFinder<UFont> FONT
	//(TEXT("Font'/Game/font/HakgyoansimKkokkomaR_Font.HakgyoansimKkokkomaR_Font'"));
	//if (FONT.Succeeded()) {
	//    loadFont = FONT.Object;
	//}
}

TArray<int32> UDPResultRoomWidget::UpdateScores(/*const TArray<FString>& names, */const TArray<int32>& scores)
{
	//if (!scoresVerticalBox)
	//	return;

	//scoresVerticalBox->ClearChildren();

	TArray<int32> sortedIndices;
	for (int32 i = 0; i < scores.Num(); i++)
		sortedIndices.Add(i);

	sortedIndices.Sort([&](int32 a, int32 b) {
		return scores[a] > scores[b];
	});

	return sortedIndices;
	//for (int32 i : sortedIndices) {
		//UHorizontalBox* playerBox = NewObject<UHorizontalBox>(this);

		//UTextBlock* nameText = NewObject<UTextBlock>(this);
		//UTextBlock* scoreText = NewObject<UTextBlock>(this);

		/*if (nameText && scoreText && playerBox) {
			nameText->SetText(FText::FromString(names[i]));
			scoreText->SetText(FText::AsNumber(scores[i]));

			FSlateFontInfo nameFontInfo = nameText->Font;
			nameFontInfo.Size = 100;
			nameText->SetFont(nameFontInfo);

			FSlateFontInfo scoreFontInfo = scoreText->Font;
			scoreFontInfo.Size = 80;
			scoreText->SetFont(scoreFontInfo);

			UHorizontalBoxSlot* nameSlot = Cast<UHorizontalBoxSlot>(playerBox->AddChild(nameText));
			UHorizontalBoxSlot* scoreSlot = Cast<UHorizontalBoxSlot>(playerBox->AddChild(scoreText));

			if (nameSlot) {
				nameSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				nameSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
				nameSlot->SetSize(ESlateSizeRule::Fill);
				nameSlot->SetPadding(FMargin(0.0f, 0.0f, 30.0f, 0.0f));
			}

			if (scoreSlot) {
				scoreSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				scoreSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
				scoreSlot->SetSize(ESlateSizeRule::Automatic);
				scoreSlot->SetPadding(FMargin(0.0f));
			}*/

			//scoresVerticalBox->AddChild(playerBox);
		//}
	//}
}
