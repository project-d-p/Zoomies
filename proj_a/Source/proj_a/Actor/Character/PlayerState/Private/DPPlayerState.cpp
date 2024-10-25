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
#include "proj_a/GameInstance/GI_Zoomies.h"

ADPPlayerState::ADPPlayerState()
{
	bReplicates = true;
	
	PlayerScoreComp = CreateDefaultSubobject<UPlayerScoreComp>(TEXT("PlayerScore"));
	PlayerScoreData = NewObject<UPlayerScoreData>(this, TEXT("PlayerScoreData"));
	PlayerScoreData->InitializeData();
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

void ADPPlayerState::IncreaseScore(const TArray<EAnimal>& InAnimals)
{
	PlayerScoreData->IncreaseScore(PlayerJob, InAnimals);
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

void ADPPlayerState::OnHostMigration(UWorld* World, UDataManager* DataManager)
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
	UPlayerScoreData* ClonedPlayerScoreData = Cast<UPlayerScoreData>(PlayerScoreData->Clone(DataManager));
	if (ClonedPlayerScoreData)
	{
		DataManager->AddDataToArray(TEXT("PlayerScore"), ClonedPlayerScoreData);
	}
}

void ADPPlayerState::InitializePlayerState()
{
 	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
 	check(GameInstance);
 	UDataManager* DataManager = GameInstance->network_failure_manager_->GetDataManager();
 	check(DataManager);
 	UDataArray* PlayerScoreDataArray = DataManager->GetDataArray(TEXT("PlayerScore"));
	if (PlayerScoreDataArray)
	{
		for (UBaseData* Data : PlayerScoreDataArray->DataArray)
		{
			UPlayerScoreData* SavedScoreData = Cast<UPlayerScoreData>(Data);
			if (SavedScoreData && SavedScoreData->GetPlayerName() == GetPlayerName())
			{
				PlayerScoreData->SetScore(SavedScoreData->GetScore());
			}
		}
	}
}

void ADPPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (!HasAuthority())
	{
		if (GameInstance)
		{
			OnHostMigrationDelegate = GameInstance->network_failure_manager_->OnHostMigration().AddUObject(this, &ADPPlayerState::OnHostMigration);
		}
	}
	
	ADPPlayerController* LocalPC = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (LocalPC)
	{
		UMainLevelComponent* MainLevelComponent = LocalPC->GetLevelComponentAs<UMainLevelComponent>(ELevelComponentType::MAIN);
		if (MainLevelComponent)
		{
			UDPIngameWidget* InGameWidget = Cast<UDPIngameWidget>(MainLevelComponent->GetInGameWidget());
			if (InGameWidget == nullptr)
			{
				FNetLogger::EditerLog(FColor::Red, TEXT("InGameWidget is nullptr"));
				return;
			}
			PlayerScoreData->OnDataChanged.AddDynamic(InGameWidget, &UDPIngameWidget::OnScoreChanged);
			PlayerScoreData->SetPlayerName(this->GetPlayerName());
			PlayerScoreData->SetPlayerId(this->GetPlayerId());
		}
	}
	InitializePlayerState();
	PlayerScoreData->TestBroadcast();
}

void ADPPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
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