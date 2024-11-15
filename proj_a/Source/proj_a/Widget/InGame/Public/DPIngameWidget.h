// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerUI.h"
#include "ScoreUI.h"
#include "../../../Component/InGame/Score/ScoreUiPrivate.h"
#include "Components/PanelWidget.h"
#include "BaseData.h"
#include "DPIngameWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API UDPIngameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnScoreChanged(UBaseData* Data);
	UFUNCTION()
	void CheckAndUpdatePlayerJob();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRankingChanged(const TArray<FString>& PlayerNameListByScore);
	
protected:
	UPROPERTY()
	UTimerUI* TimerUI = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Time_Text;
	
	UPROPERTY()
	UScoreUiPrivate* ScoreUI_Private = nullptr;
	//ScoreBox
	UPROPERTY()
	UTextBlock* scoreFront = nullptr;
	UPROPERTY()
	UTextBlock* scoreBack = nullptr;
	UPROPERTY()
	UTextBlock* scoreJob = nullptr;
	UPROPERTY()
	UTextBlock* scoreTotal = nullptr;

	void UpdateTextBlock(UTextBlock* TextBlock);
	void FindAndUpdateTextBlocks(UWidget* ParentWidget);

	int32 reCallCnt = 0;

private:
	void ChangeListByScore();
	
	TArray<FString> PlayerNameListByScore;
};
