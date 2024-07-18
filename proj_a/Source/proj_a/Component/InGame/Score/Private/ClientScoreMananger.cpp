#include "ClientScoreMananger.h"

#include "DPInGameState.h"
#include "FNetLogger.h"
#include "PlayerScoreComp.h"
#include "Net/UnrealNetwork.h"

UClientScoreMananger::UClientScoreMananger()
{
	SetIsReplicatedByDefault(true);
}

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
	ScoreUI = InScoreUI;
}

void UClientScoreMananger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UClientScoreMananger, ScoreUI);
}
