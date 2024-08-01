#pragma once

#include "CoreMinimal.h"
#include "ScoreUI.h"
#include "TimerUI.h"
#include "VoteWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "JudgeLevelUI.generated.h"

UENUM()
enum class ETextBlockType : uint8
{
	Id,
	Score,
	Occupation
};

UCLASS()
class PROJ_A_API UJudgeLevelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetBlockContent(ETextBlockType BlockType, int32 Index, const FString& Content);
	EPlayerJob GetVote() const { return widget_vote->GetVote(); }
	void SetVoterName(const FString& Name) { widget_vote->SetVoterName(Name); }

protected:
	void InitTextBlocksFromContainer(UPanelWidget* Container, TArray<UTextBlock*>& OutTextBlocks);
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	UTimerUI* TimerUI = nullptr;
	UPROPERTY()
	UScoreUI* ScoreUI = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Time_Text;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* IdContainer;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ScoreContainer;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* OccupationContainer;
	UPROPERTY(meta = (BindWidget))
	UVoteWidget* widget_vote = nullptr;

	UPROPERTY()
	TArray<UTextBlock*> IdBlocks;
	UPROPERTY()
	TArray<UTextBlock*> ScoreBlocks;
	UPROPERTY()
	TArray<UTextBlock*> OccupationBlocks;

	TMap<ETextBlockType, TArray<UTextBlock*>> TextBlockMap;
};
