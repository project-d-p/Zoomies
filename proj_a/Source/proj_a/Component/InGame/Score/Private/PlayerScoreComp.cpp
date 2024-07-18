#include "PlayerScoreComp.h"

#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"

UPlayerScoreComp::UPlayerScoreComp()
{
	SetIsReplicatedByDefault(true);
}

void UPlayerScoreComp::OnRep_PlayerScores()
{
	check(PlayerScore_Text)
	PlayerScore_Text->SetText(FText::AsNumber(PlayerScores));
}

void UPlayerScoreComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UPlayerScoreComp, PlayerScores);
}
