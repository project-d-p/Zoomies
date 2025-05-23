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
	// ADPGameModeBase* GM = UGameHelper::GetInGameMode(GetWorld());
	ADPGameModeBase* GM = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->ScoreManager->IncreasePlayerScore(GetWorld()->GetFirstPlayerController(), { EAnimal::ANIMAL_WHALE, EAnimal::ANIMAL_ELEPHANT, EAnimal::ANIMAL_MAMMOTH});
	}

	ADPPlayerController* PC = Cast<ADPPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->GetPrivateScoreManagerComponent()->IncreasePrivatePlayerScore(EPlayerJob::JOB_ARCHAEOLOGIST,{ EAnimal::ANIMAL_WHALE, EAnimal::ANIMAL_ELEPHANT, EAnimal::ANIMAL_MAMMOTH});
	}
}

void UMyUserWidget::OnScoreBT2Clicked()
{
	ADPGameModeBase* GM = Cast<ADPGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
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
		GM->ScoreManager->IncreasePlayerScore(SecondPlayerController, { EAnimal::ANIMAL_WHALE, EAnimal::ANIMAL_ELEPHANT, EAnimal::ANIMAL_MAMMOTH});
	}
}
