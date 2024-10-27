// Fill out your copyright notice in the Description page of Project Settings.


#include "DPIngameWidget.h"

#include "DPInGameState.h"
#include "DPPlayerState.h"
#include "FNetLogger.h"
#include "PlayerScoreComp.h"
#include "Components/TextBlock.h"
#include "../../../Component/InGame/Score/ScoreUiPrivate.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/PlayerState.h"

void UDPIngameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TimerUI = NewObject<UTimerUI>(Time_Text);
	if (TimerUI)
	{
		FTimerUiInitializer TimerUiInitializer;
		TimerUiInitializer.Time_Text = Time_Text;
		TimerUiInitializer.InWorld = GetWorld();
		TimerUI->initTimerUI<ADPInGameState>(TimerUiInitializer);
	}
	
	FindAndUpdateTextBlocks(this);

	ScoreUI_Private = NewObject<UScoreUiPrivate>(this);
	if (ScoreUI_Private)
	{
		FPrivateScoreUiInitializer PrivateScoreUiInitializer;
		PrivateScoreUiInitializer.InWorld = GetWorld();
		//PrivateScoreUiInitializer.ScoreTextPrivate = score_Text_Private;
		PrivateScoreUiInitializer.ScoreTextPrivate_Front = scoreFront;
		PrivateScoreUiInitializer.ScoreTextPrivate_Back = scoreBack;
		PrivateScoreUiInitializer.ScoreTextPrivate_Job = scoreJob;
		PrivateScoreUiInitializer.ScoreTextPrivate_Total = scoreTotal;
		ScoreUI_Private->InitScoreUiPrivate(PrivateScoreUiInitializer);
	}
}
void UDPIngameWidget::FindAndUpdateTextBlocks(UWidget* ParentWidget)
{
	if (!ParentWidget)
		return;

	if (UTextBlock* TextBlock = Cast<UTextBlock>(ParentWidget))
	{
		if (TextBlock->GetName() == "Text_Title")
		{
			UpdateTextBlock(TextBlock);
		}
	}

	if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(ParentWidget))
	{
		for (int32 i = 0; i < PanelWidget->GetChildrenCount(); ++i)
		{
			UWidget* ChildWidget = PanelWidget->GetChildAt(i);
			FindAndUpdateTextBlocks(ChildWidget);
		}
	}
	else if (UUserWidget* UserWidget = Cast<UUserWidget>(ParentWidget))
	{
		UWidgetTree* LocalWidgets = UserWidget->WidgetTree;
		if (LocalWidgets)
		{
			TArray<UWidget*> AllWidgets;
			LocalWidgets->GetAllWidgets(AllWidgets);
			for (UWidget* ChildWidget : AllWidgets)
			{
				FindAndUpdateTextBlocks(ChildWidget);
			}
		}
	}
}

void UDPIngameWidget::UpdateTextBlock(UTextBlock* TextBlock)
{
	if (!TextBlock)
		return;

	UObject* Outer = TextBlock->GetOuter()->GetOuter();
	FString OuterName = Outer ? Outer->GetName() : "Unknown";

	if (OuterName.Contains("WBP_InGame_ScoreBox_Front"))
	{
		scoreFront = TextBlock;
	}
	else if (OuterName.Contains("WBP_InGame_ScoreBox_Back"))
	{
		scoreBack = TextBlock;
	}
	else if (OuterName.Contains("WBP_InGame_ScoreBox_Job"))
	{
		scoreJob = TextBlock;
		FText JobText = scoreJob->GetText();
		FText DefaultJob = FText::FromString("Job");
		if (JobText.EqualTo(DefaultJob))
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
			{
				CheckAndUpdatePlayerJob();
			});
		}
	}
	else if (OuterName.Contains("WBP_InGame_ScoreBox_Total"))
	{
		scoreTotal = TextBlock;
	}
	else
	{
		TextBlock->SetText(FText::FromString("else"));
	}
}

void UDPIngameWidget::CheckAndUpdatePlayerJob()
{
	ADPPlayerState* P = Cast<ADPPlayerState>(GetOwningPlayerState());
	if (P)
	{
		EPlayerJob Job = P->GetPlayerJob();
		FString JobString = UEnum::GetValueAsString<EPlayerJob>(Job);
		JobString = JobString.Replace(TEXT("EPlayerJob::JOB_"), TEXT(""));
		scoreJob->SetText(FText::FromString(JobString));
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			CheckAndUpdatePlayerJob();
		});
	}
}

void UDPIngameWidget::OnScoreChanged(UBaseData* Data)
{
	//BaseData to PlayerScoreData
	UPlayerScoreData* PlayerScoreData = Cast<UPlayerScoreData>(Data);
	
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	if (PlayerController)
	{
		ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
		if (!PlayerState)
		{
			if (reCallCnt < 10)
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick([this, Data]()
				{
					reCallCnt++;
					OnScoreChanged(Data);
				});
			}
		}
		else
		{
			reCallCnt = 0;
			if (PlayerState->GetPlayerId() == PlayerScoreData->GetPlayerId())
			{
				if (!PlayerScoreData)
				{
					//FNetLogger::EditerLog(FColor::Red, TEXT("OnScoreChanged: PlayerScoreData is nullptr"));
					return;
				}
				if (scoreFront == nullptr || scoreBack == nullptr || scoreTotal == nullptr)
				{
					//FNetLogger::EditerLog(FColor::Red, TEXT("OnScoreChanged: scoreFront or scoreBack or scoreTotal is nullptr"));
					return;
				}
					
				FFinalScoreData ScoreData = PlayerScoreData->GetScore();
				scoreFront->SetText(FText::FromString(FString::FromInt(ScoreData.PrivateTotalBaseScore)));
				scoreBack->SetText(FText::FromString(FString::FromInt(ScoreData.PrivateTotalScale)));
				scoreTotal->SetText(FText::FromString(FString::FromInt(ScoreData.PrivateTotalScore)));
			}
		}
	}
}
