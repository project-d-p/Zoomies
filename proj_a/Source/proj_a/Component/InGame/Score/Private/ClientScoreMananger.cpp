#include "ClientScoreMananger.h"

#include "DPInGameState.h"
#include "FNetLogger.h"
#include "PlayerScoreComp.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UClientScoreMananger::UClientScoreMananger()
{
	SetIsReplicatedByDefault(true);
}

void UClientScoreMananger::InitScoreUi()
{
	ADPPlayerState* MyPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ADPPlayerState>();
	if (MyPlayerState == nullptr)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("PlayerState is not found!"));
		return;
	}
	TArray<TObjectPtr<APlayerState>> PlayerStates = Cast<ADPInGameState>(GetOwner())->PlayerArray;
	//
	for (APlayerState* PlayerState : PlayerStates)
	{
		FNetLogger::EditerLog(FColor::Blue, TEXT("PlayerState: %d"), PlayerState->GetPlayerId());
		UTextBlock* TextBlock;
		if (ScoreUI == nullptr)
		{
			FNetLogger::EditerLog(FColor::Red, TEXT("ScoreUI is not found!"));
			return;
		}
		if (Cast<ADPPlayerState>(PlayerState)->GetUniqueID() == MyPlayerState->GetUniqueID())
		{
			Cast<ADPPlayerState>(PlayerState)->GetPlayerScoreComp()->PlayerScore_Text = ScoreUI->GetPlayerScoreTexts();
		}
		else
		{
			TextBlock = ScoreUI->FindEmptyTextBlock();
			if (TextBlock == nullptr)
			{
				FNetLogger::EditerLog(FColor::Red, TEXT("TextBlock is not found!"));
				return;
			}
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
