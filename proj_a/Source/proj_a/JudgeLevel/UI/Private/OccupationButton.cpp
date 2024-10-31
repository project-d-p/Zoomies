#include "OccupationButton.h"

#include "PathManager.h"
#include "VoteWidget.h"
#include "Components/SizeBox.h"

UOccupationButton::UOccupationButton()
{
	OnClicked.AddDynamic(this, &UOccupationButton::InternalOnClicked);
}

void UOccupationButton::InternalOnClicked()
{
	check(OnClickedCallback)
	OnClickedCallback(Occupation);

	UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(Occupation));
	if (NewTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(NewTexture);
		NewBrush.ImageSize = FVector2D(64, 64);
		WidgetStyle.Normal = NewBrush;
	}

	UVoteWidget* VoteWidget = Cast<UVoteWidget>(GetOuter()->GetOuter());
	if (VoteWidget)
	{
		for (int32 i = 0; i < VoteWidget->OccupationTypes.Num(); ++i)
		{
			if (VoteWidget->OccupationTypes[i] == Occupation) continue;
			UWidget* ChildWidget = VoteWidget->VoteButtonsGrid->GetChildAt(i);
			UTexture2D* NewTexture_White = LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath_White(VoteWidget->OccupationTypes[i]));
			if (NewTexture_White)
			{
				FSlateBrush NewBrush;
				NewBrush.SetResourceObject(NewTexture_White);
				NewBrush.ImageSize = FVector2D(64, 64);
				if (USizeBox* SizeBox = Cast<USizeBox>(ChildWidget))
				{
					if (UButton* Button = Cast<UButton>(SizeBox->GetChildAt(0)))
					{
						if (UOccupationButton* OccupationButton = Cast<UOccupationButton>(Button))
						{
							OccupationButton->WidgetStyle.Normal = NewBrush;
						}
					}
				}
			}
		}
	}
}
