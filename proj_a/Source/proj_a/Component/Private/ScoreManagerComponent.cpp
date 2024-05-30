#include "ScoreManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "FNetLogger.h"

UScoreManagerComponent::UScoreManagerComponent()
{
	SetIsReplicatedByDefault(true);
}

void UScoreManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	Player0Scores = 0;
	Player1Scores = 0;
}

void UScoreManagerComponent::IncreasePlayer0Score_Implementation(int32 Amount)
{
	Player0Scores += Amount;
	FNetLogger::EditerLog(FColor::Green, TEXT("Increase Score: PlayerIndex(0), Amount(%d)"), Player0Scores);
}

void UScoreManagerComponent::IncreasePlayer1Score_Implementation(int32 Amount)
{
	Player1Scores += Amount;
	FNetLogger::EditerLog(FColor::Green, TEXT("Increase Score: PlayerIndex(1), Amount(%d)"), Player1Scores);
}

int32 UScoreManagerComponent::GetScore(int32 PlayerIndex) const
{
	// if (PlayerIndex >= 0 && PlayerIndex < 4)
	// {
	// 	return PlayerScores[PlayerIndex];
	// }
	return 0;
}

void UScoreManagerComponent::OnRep_PlayerScoresUpdated()
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Player Scores Updated"));
	FNetLogger::EditerLog(FColor::Green, TEXT("Player %d Score: %d"), 0, Player0Scores);
	FNetLogger::EditerLog(FColor::Green, TEXT("Player %d Score: %d"), 1, Player1Scores);

	// UI 업데이트 로직 추가
}

void UScoreManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UScoreManagerComponent, Player0Scores);
	DOREPLIFETIME(UScoreManagerComponent, Player1Scores);
}