#include "JudgeLevelUI.h"

#include "DPInGameState.h"
#include "FNetLogger.h"
#include "JudgeGameState.h"

void UJudgeLevelUI::NativeConstruct()
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
		ScoreUiInitializer.ScoreText = player1scoreText;
		ScoreUiInitializer.Player1ScoreText = player2ScoreText;
		ScoreUiInitializer.Player2ScoreText = player3ScoreText;
		ScoreUiInitializer.Player3ScoreText = player4ScoreText;
		ScoreUI->InitScoreUi(ScoreUiInitializer);
	}
	//
	// ScoreUI_Private = NewObject<UScoreUiPrivate>(this);
	// if (ScoreUI_Private)
	// {
	// 	FPrivateScoreUiInitializer PrivateScoreUiInitializer;
	// 	PrivateScoreUiInitializer.InWorld = GetWorld();
	// 	PrivateScoreUiInitializer.ScoreTextPrivate = score_Text_Private;
	// 	ScoreUI_Private->InitScoreUiPrivate(PrivateScoreUiInitializer);
	// }
}
