#include "ScoreManager.h"
#include "Net/UnrealNetwork.h"

UScoreManager::UScoreManager()
{
	SetIsReplicatedByDefault(true);
}

void UScoreManager::BeginPlay()
{
	Super::BeginPlay();
}

void UScoreManager::IncreasePlayerScore_Implementation(int32 Amount)
{
	PlayerScores += Amount;
}

void UScoreManager::OnRep_PlayerScoresUpdated()
{
	// UI 업데이트 로직 추가
}

void UScoreManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UScoreManager, PlayerScores);
}