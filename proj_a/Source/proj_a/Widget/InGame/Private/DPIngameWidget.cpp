// Fill out your copyright notice in the Description page of Project Settings.


#include "DPIngameWidget.h"

#include "DPPlayerState.h"
#include "FNetLogger.h"
#include "PlayerScoreComp.h"
#include "Components/TextBlock.h"
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
		TimerUI->initTimerUI(TimerUiInitializer);
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
}