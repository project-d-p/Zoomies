// Fill out your copyright notice in the Description page of Project Settings.


#include "DPIngameWidget.h"

#include "DPInGameState.h"
#include "DPPlayerState.h"
#include "FNetLogger.h"
#include "PlayerScoreComp.h"
#include "Components/TextBlock.h"
#include "../../../Component/InGame/Score/ScoreUiPrivate.h"
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
	
	ScoreUI = NewObject<UScoreUI>(this);
	if (ScoreUI)
	{
		FScoreUiInitializer ScoreUiInitializer;
		ScoreUiInitializer.InWorld = GetWorld();
		ScoreUiInitializer.ScoreText = score_Text;
		ScoreUiInitializer.Player1ScoreText = player1Score_Text;
		ScoreUiInitializer.Player2ScoreText = player2Score_Text;
		ScoreUiInitializer.Player3ScoreText = player3Score_Text;
		ScoreUI->InitScoreUi(ScoreUiInitializer);
	}

	ScoreUI_Private = NewObject<UScoreUiPrivate>(this);
	if (ScoreUI_Private)
	{
		FPrivateScoreUiInitializer PrivateScoreUiInitializer;
		PrivateScoreUiInitializer.InWorld = GetWorld();
		PrivateScoreUiInitializer.ScoreTextPrivate = score_Text_Private;
		ScoreUI_Private->InitScoreUiPrivate(PrivateScoreUiInitializer);
	}
}

void UDPIngameWidget::OnScoreChanged(const UBaseData* Data)
{
	FNetLogger::EditerLog(FColor::Cyan, TEXT("OnScoreChanged"));
}
