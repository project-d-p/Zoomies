#include "PlayerScoreComp.h"

#include "DPInGameState.h"
#include "FNetLogger.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

UPlayerScoreComp::UPlayerScoreComp()
{
	SetIsReplicatedByDefault(true);
}

void UPlayerScoreComp::OnRep_PlayerScores()
{
	APlayerState* PS = Cast<APlayerState>(GetOwner());

	if (PlayerScore_Text == nullptr)
	{
		// FNetLogger::EditerLog(FColor::Red, TEXT("PlayerScore_Text is nullptr in PS"));
		Cast<ADPInGameState>(GetWorld()->GetGameState())->ScoreManager->InitScoreUi();
		return;
	}
	// XXX: ���� ScoreUI���� ������Ʈ �ϴ� �������� ����.
	PlayerScore_Text->SetText(FText::AsNumber(PlayerScores));
}

void UPlayerScoreComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UPlayerScoreComp, PlayerScores);
}
