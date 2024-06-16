#include "MyUserWidget.h"

#include "DPGameModeBase.h"
#include "DPInGameState.h"
#include "FNetLogger.h"
#include "GameHelper.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"

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
		GM->ScoreManager->IncreasePlayerScore(GetWorld()->GetFirstPlayerController(), { EAnimal::ANIMAL_DEER, EAnimal::ANIMAL_COW });
	}

	ADPPlayerController* PC = Cast<ADPPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->GetPrivateScoreManagerComponent()->IncreasePrivatePlayerScore(EPlayerJob::JOB_HUNTER,{ EAnimal::ANIMAL_DEER, EAnimal::ANIMAL_COW });
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
		//Text EAnimalS Array
		GM->ScoreManager->IncreasePlayerScore(SecondPlayerController, { EAnimal::ANIMAL_DEER, EAnimal::ANIMAL_COW });
	}
}
