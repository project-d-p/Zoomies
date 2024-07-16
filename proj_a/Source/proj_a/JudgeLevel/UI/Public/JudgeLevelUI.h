#pragma once

#include "CoreMinimal.h"
#include "ScoreUI.h"
#include "TimerUI.h"
#include "Blueprint/UserWidget.h"
#include "proj_a/Component/InGame/Score/ScoreUiPrivate.h"
#include "JudgeLevelUI.generated.h"

UCLASS()
class PROJ_A_API UJudgeLevelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
protected:
	UPROPERTY()
	UTimerUI* TimerUI = nullptr;
	// UPROPERTY()
	// UScoreUI* ScoreUI = nullptr;
	// UPROPERTY()
	// UScoreUiPrivate* ScoreUI_Private = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Time_Text;

	// UPROPERTY(meta = (BindWidget))
	// UTextBlock* score_Text;
	// UPROPERTY(meta = (BindWidget))
	// UTextBlock* score_Text_Private;
	// UPROPERTY(meta = (BindWidget))
	// UTextBlock* player1Score_Text;
	// UPROPERTY(meta = (BindWidget))
	// UTextBlock* player2Score_Text;
	// UPROPERTY(meta = (BindWidget))
	// UTextBlock* player3Score_Text;
};
