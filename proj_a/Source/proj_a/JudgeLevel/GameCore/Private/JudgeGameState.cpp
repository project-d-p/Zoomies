#include "JudgeGameState.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "JudgeData.h"
#include "JudgeLevelComponent.h"
#include "JudgePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AJudgeGameState::AJudgeGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
	JudgedInformation = NewObject<UJudgeData>(this, TEXT("JudgedInformation"));
	JudgedInformation->InitializeData();
    static ConstructorHelpers::FObjectFinder<USoundWave> SoundAsset(TEXT("/Game/sounds/strange_bell.strange_bell"));
    if (SoundAsset.Succeeded())
    {
        TurnStartSound = SoundAsset.Object;
    }
}

void AJudgeGameState::NotifyCurrentPlayerVoted_Implementation(const FString& VotedPlayerName)
{
	JudgedInformation->AddJudgedPlayerName(VotedPlayerName);
}

void AJudgeGameState::NotifyTimerEnd_Implementation()
{
	AJudgePlayerController* PC = Cast<AJudgePlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->GetJudgeLevelUI())
		return ;
	PC->ReturnVote(CurrentVotedPlayerName, PC->GetJudgeLevelUI()->GetVote());
}

void AJudgeGameState::SetVoterName_Implementation(const FString& Name)
{
	AJudgePlayerController* PC = Cast<AJudgePlayerController>(GetWorld()->GetFirstPlayerController());
	check(PC)
	PC->GetJudgeLevelUI()->SetVoterName(Name);
	CurrentVotedPlayerName = Name;
	if (TurnStartSound)
	{
		UGameplayStatics::PlaySound2D(this, TurnStartSound);
	}
}

void AJudgeGameState::BeginPlay()
{
	Super::BeginPlay();

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	check(GameInstance)
	if (!HasAuthority())
	{
		if (GameInstance)
		{
			OnHostMigrationDelegate = GameInstance->network_failure_manager_->OnHostMigration().AddUObject(this, &AJudgeGameState::OnHostMigration);
		}
	}
}

void AJudgeGameState::OnHostMigration(UWorld* World, UDataManager* DataManager)
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
	UJudgeData* ClonedPlayerScoreData = Cast<UJudgeData>(JudgedInformation->Clone(DataManager));
	if (ClonedPlayerScoreData)
	{
		DataManager->AddDataToSingle(TEXT("JudgeData"), ClonedPlayerScoreData);
	}
}

void AJudgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJudgeGameState, TimerManager);
	DOREPLIFETIME(AJudgeGameState, GS_PlayerData);
}

