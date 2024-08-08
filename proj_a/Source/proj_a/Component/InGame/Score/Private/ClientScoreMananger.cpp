#include "ClientScoreMananger.h"

#include "DPInGameState.h"
#include "DPPlayerState.h"
#include "PlayerScoreComp.h"

void UClientScoreMananger::InitScoreUi()
{
	ADPPlayerState* PS = GetWorld()->GetFirstPlayerController()->GetPlayerState<ADPPlayerState>();
	check(PS);
	TArray<TObjectPtr<APlayerState>> PlayerStates = Cast<ADPInGameState>(GetOwner())->PlayerArray;
	for (APlayerState* PlayerState : PlayerStates)
	{
		UTextBlock* TextBlock;
		check(ScoreUI)
		if (Cast<ADPPlayerState>(PlayerState)->GetUniqueID() == PS->GetUniqueID())
		{
			Cast<ADPPlayerState>(PlayerState)->GetPlayerScoreComp()->PlayerScore_Text = ScoreUI->GetPlayerScoreTexts();
		}
		else
		{
			TextBlock = ScoreUI->FindEmptyTextBlock();
			check(TextBlock)
			Cast<ADPPlayerState>(PlayerState)->GetPlayerScoreComp()->PlayerScore_Text = TextBlock;
		}
	}
}

void UClientScoreMananger::SetScoreUI(UScoreUI* InScoreUI)
{
	if (ScoreUI != nullptr)
	{
		ScoreUI->DestroyComponent();
		ScoreUI = InScoreUI;
	}
	else
	{
		ScoreUI = InScoreUI;
	}
}