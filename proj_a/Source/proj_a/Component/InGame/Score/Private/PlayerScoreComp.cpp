#include "PlayerScoreComp.h"

#include "DPInGameState.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"

UPlayerScoreComp::UPlayerScoreComp()
{
	SetIsReplicatedByDefault(true);
}

void UPlayerScoreComp::OnRep_PlayerScores()
{
	if (PlayerScore_Text)
	{
		PlayerScore_Text->SetText(FText::AsNumber(PlayerScores));
	}
	else
	{
		ADPInGameState* GS = Cast<ADPInGameState>(GetWorld()->GetGameState());
		check(GS)
		GS->ScoreManager->InitScoreUi();
	}
}

void UPlayerScoreComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UPlayerScoreComp, PlayerScores);
}
