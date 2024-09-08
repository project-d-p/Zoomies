#include "DPPlayerState.h"

#include "DPInGameState.h"
#include "FNetLogger.h"
#include "JudgePlayerState.h"
#include "PlayerName.h"
#include "PlayerScoreComp.h"
#include "Net/UnrealNetwork.h"

ADPPlayerState::ADPPlayerState()
{
	bReplicates = true;
	
	PlayerScoreComp = CreateDefaultSubobject<UPlayerScoreComp>(TEXT("PlayerScore"));
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

void ADPPlayerState::OnRep_Rank()
{
}

void ADPPlayerState::OnRep_Job()
{
	// switch (PlayerJob)
	// {
	// case EPlayerJob::JOB_ARCHAEOLOGIST:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_ARCHAEOLOGIST"));
	// 	break;
	// case EPlayerJob::JOB_POACHER:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_POACHER"));
	// 	break;
	// case EPlayerJob::JOB_RINGMASTER:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_RINGMASTER"));
	// 	break;
	// case EPlayerJob::JOB_TERRORIST:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_TERRORIST"));
	// 	break;
	// case EPlayerJob::JOB_ENVIRONMENTALIST:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_ENVIRONMENTALIST"));
	// 	break;
	// default:
	// 	break;
	// }
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

	OnRep_Rank();
}

void ADPPlayerState::BeginPlay()
{
	Super::BeginPlay();
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
	// switch (PlayerJob)
	// {
	// case EPlayerJob::JOB_ARCHAEOLOGIST:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_ARCHAEOLOGIST"));
	// 	break;
	// case EPlayerJob::JOB_POACHER:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_POACHER"));
	// 	break;
	// case EPlayerJob::JOB_RINGMASTER:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_RINGMASTER"));
	// 	break;
	// case EPlayerJob::JOB_TERRORIST:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_TERRORIST"));
	// 	break;
	// case EPlayerJob::JOB_ENVIRONMENTALIST:
	// 	FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Job : JOB_ENVIRONMENTALIST"));
	// 	break;
	// default:
	// 	break;
	// }
}