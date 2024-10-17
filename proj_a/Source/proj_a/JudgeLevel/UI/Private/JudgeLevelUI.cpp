#include "JudgeLevelUI.h"

#include "FNetLogger.h"
#include "JudgeGameState.h"

void UJudgeLevelUI::SetBlockContent(ETextBlockType BlockType, int32 Index, const FString& Content)
{
	TArray<UTextBlock*>* Blocks = TextBlockMap.Find(BlockType);
	checkf(Blocks, TEXT("Invalid BlockType"));
	checkf(Blocks->IsValidIndex(Index) && (*Blocks)[Index], TEXT("Invalid index or null TextBlock"));
	(*Blocks)[Index]->SetText(FText::FromString(Content));
}

void UJudgeLevelUI::InitTextBlocksFromContainer(UPanelWidget* Container, TArray<UTextBlock*>& OutTextBlocks)
{
}

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
}
