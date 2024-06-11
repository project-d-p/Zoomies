#include "ScoreUI.h"

#include "DPInGameState.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"

void UScoreUI::InitScoreUi(FScoreUiInitializer& Initializer)
{
	ADPInGameState* GS = Cast<ADPInGameState>(Initializer.InWorld->GetGameState());
	if (GS)
	{
		GS->ScoreManager->SetScoreUI(this);
	}
	ScoreText = Initializer.ScoreText;
	PlayerScoreTexts.Add({Initializer.Player1ScoreText, false});
	PlayerScoreTexts.Add({Initializer.Player2ScoreText, false});
	PlayerScoreTexts.Add({Initializer.Player3ScoreText, false});
}

UTextBlock* UScoreUI::FindEmptyTextBlock()
{
	for (FTextBlockStruct& TextBlockStruct : PlayerScoreTexts)
	{
		if (TextBlockStruct.bIsSet == false)
		{
			TextBlockStruct.bIsSet = true;
			return TextBlockStruct.TextBlock;
		}
	}
	return nullptr;
}

UTextBlock* UScoreUI::GetPlayerScoreTexts() const
{
	return ScoreText;
}
