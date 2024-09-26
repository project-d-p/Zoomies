#include "DPPlayerState.h"

#include "DPInGameState.h"
#include "DPIngameWidget.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "JudgePlayerState.h"
#include "PlayerName.h"
#include "PlayerScoreComp.h"
#include "PlayerScoreData.h"
#include "Net/UnrealNetwork.h"

ADPPlayerState::ADPPlayerState()
{
	bReplicates = true;
	
	PlayerScoreComp = CreateDefaultSubobject<UPlayerScoreComp>(TEXT("PlayerScore"));
	PlayerScoreData = NewObject<UPlayerScoreData>(this, TEXT("PlayerScoreData"));
	FString PlayerName = FGuid::NewGuid().ToString();
	APlayerState::SetPlayerName(PlayerName);
}

UPlayerScoreComp* ADPPlayerState::GetPlayerScoreComp() const
{
	return PlayerScoreComp;
}

EPlayerJob ADPPlayerState::GetPlayerJob() const
{
	return PlayerJob;
}

void ADPPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	AJudgePlayerState* JPS = Cast<AJudgePlayerState>(PlayerState);
	if (JPS)
	{
		JPS->SetPlayerJob(PlayerJob);
	}
}

void ADPPlayerState::ServerSetRank_Implementation(int InRank)
{
	Rank = InRank;
}

void ADPPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ADPPlayerController* LocalPC = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (LocalPC)
	{
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(LocalPC->GetLevelComponent());
		if (MainLevelComponent)
		{
			UDPIngameWidget* InGameWidget = Cast<UDPIngameWidget>(MainLevelComponent->GetInGameWidget());
			PlayerScoreData->OnDataChanged.AddDynamic(InGameWidget, &UDPIngameWidget::OnScoreChanged);
		}
	}
}

void ADPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPPlayerState, PlayerJob);
	DOREPLIFETIME(ADPPlayerState, Rank);
}

void ADPPlayerState::SetPlayerRandomJob()
{
	PlayerJob = static_cast<EPlayerJob>(FMath::RandRange(0, static_cast<int>(EPlayerJob::JOB_MAX) - 1));
}