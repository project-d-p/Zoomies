#include "MyUserWidget.h"

#include "DPGameModeBase.h"
#include "DPInGameState.h"
#include "FNetLogger.h"
#include "GameHelper.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ScoreBT1)
	{
		ScoreBT1->OnClicked.AddDynamic(this, &UMyUserWidget::OnScoreBT1Clicked);
	}
	if (ScoreBT2)
	{
		ScoreBT2->OnClicked.AddDynamic(this, &UMyUserWidget::OnScoreBT2Clicked);
	}
}

void UMyUserWidget::OnScoreBT1Clicked()
{
	ADPGameModeBase* GM = UGameHelper::GetInGameMode(GetWorld());
	if (GM)
	{
		GM->ScoreManager->IncreasePlayerScore(GetWorld()->GetFirstPlayerController(), 10);
	}
}

void UMyUserWidget::OnScoreBT2Clicked()
{
	ADPGameModeBase* GM = UGameHelper::GetInGameMode(GetWorld());
	if (GM)
	{
		int32 ControllerIndex = 0;
		APlayerController* SecondPlayerController = nullptr;
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ControllerIndex == 1)
			{
				SecondPlayerController = Iterator->Get();
				break;
			}
			++ControllerIndex;
		}
		GM->ScoreManager->IncreasePlayerScore(SecondPlayerController, 10);
	}
}
