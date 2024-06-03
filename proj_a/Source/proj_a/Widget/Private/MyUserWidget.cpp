#include "MyUserWidget.h"

#include "DPInGameState.h"
#include "FNetLogger.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
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
	UWorld* World = GetWorld();
	if (World)
	{
		ADPInGameState* GameState = Cast<ADPInGameState>(UGameplayStatics::GetGameState(World));
		if (GameState)
		{
			FNetLogger::EditerLog(FColor::Purple, TEXT("ScoreBT1 Clicked"));
			GameState->ScoreManagerComponent->IncreasePlayer0Score(10);
		}
		else
		{
			FNetLogger::EditerLog(FColor::Red, TEXT("GameState is null"));
		}
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("World is null"));
	}
}

void UMyUserWidget::OnScoreBT2Clicked()
{
	
	UWorld* World = GetWorld();
	if (World)
	{
		ADPInGameState* GameState = Cast<ADPInGameState>(UGameplayStatics::GetGameState(World));
		if (GameState)
		{
			GameState->ScoreManagerComponent->IncreasePlayer0Score(10);
		}
	}
}
