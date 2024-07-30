#include "DPResultWidget.h"


void UDPResultWidget::ShowEndGame()
{
	this->OnResultTextSet();
}

void UDPResultWidget::InitSingleTextBlock(UHorizontalBox* HorizontalBox, UTextBlock* TextBlock)
{
	UWidget* Child = HorizontalBox->GetChildAt(0);
	if (UTextBlock* Text = Cast<UTextBlock>(Child))
	{
		TextBlock = Text;
	}
}

void UDPResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitSingleTextBlock(PlayerNameContainer, PlayerNameText);
	InitSingleTextBlock(PlayerRankContainer, PlayerRankText);
}
