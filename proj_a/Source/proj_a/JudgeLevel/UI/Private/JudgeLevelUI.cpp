#include "JudgeLevelUI.h"

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
		TimerUI->initTimerUI<AJudgeGameState>(TimerUiInitializer);
	}
	
	// ScoreUI = NewObject<UScoreUI>(this);
	// if (ScoreUI)
	// {
	// 	FScoreUiInitializer ScoreUiInitializer;
	// 	ScoreUiInitializer.InWorld = GetWorld();
	// 	ScoreUiInitializer.ScoreText = score_Text;
	// 	ScoreUiInitializer.Player1ScoreText = player1Score_Text;
	// 	ScoreUiInitializer.Player2ScoreText = player2Score_Text;
	// 	ScoreUiInitializer.Player3ScoreText = player3Score_Text;
	// 	ScoreUI->InitScoreUi(ScoreUiInitializer);
	// }
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
