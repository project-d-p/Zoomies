#include "DPPlayerState.h"

#include "DPInGameState.h"
#include "DPIngameWidget.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "JudgePlayerState.h"
#include "PlayerName.h"
#include "PlayerScoreComp.h"
#include "PlayerScoreData.h"
#include "Net/OnlineEngineInterface.h"
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
	// SetSessionName();
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

void ADPPlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);

	AJudgePlayerState* JPS = Cast<AJudgePlayerState>(NewPlayerState);
	if (JPS)
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("SeamlessTravel To in PlayerState"));
		JPS->SetPlayerJob(PlayerJob);
		JPS->SetPlayerScoreData(PlayerScoreData);
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Seamless: %s"), *(JPS->GetPlayerScoreData()->GetPlayerName()));
	}
}

void ADPPlayerState::RegisterPlayerWithSession(bool bWasFromInvite)
{
	SetSessionName();
	
	Super::RegisterPlayerWithSession(bWasFromInvite);

	if (GetNetMode() != NM_Standalone)
	{
		if (GetUniqueId().IsValid()) // May not be valid if this is was created via DebugCreatePlayer
		{
			// Register the player as part of the session
			if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), this->SessionName))
			{
				UOnlineEngineInterface::Get()->RegisterPlayer(GetWorld(), this->SessionName, GetUniqueId(), bWasFromInvite);
			}
		}
	}
}

void ADPPlayerState::UnregisterPlayerWithSession()
{
	SetSessionName();
	
	Super::UnregisterPlayerWithSession();

	if (GetNetMode() == NM_Client && GetUniqueId().IsValid())
	{
		if (this->SessionName != NAME_None)
		{
			if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), this->SessionName))
			{
				UOnlineEngineInterface::Get()->UnregisterPlayer(GetWorld(), this->SessionName, GetUniqueId());
			}
		}
	}
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

void ADPPlayerState::AddInGameWidgetFunctionToDelegate()
{
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
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					AddInGameWidgetFunctionToDelegate();
				}, 0.5f, false);
			}
			else
			{
				PlayerScoreData->OnDataChanged.AddDynamic(InGameWidget, &UDPIngameWidget::OnScoreChanged);
				PlayerScoreData->TestBroadcast();
			}
		}
	}
}

void ADPPlayerState::InitializePlayerState()
{
	if (GetWorld()->GetMapName().Contains(TEXT("mainLevel")))
	{
		AddInGameWidgetFunctionToDelegate();
	}
	
	PlayerScoreData->SetPlayerName(this->GetPlayerName());
	PlayerScoreData->SetPlayerId(this->GetPlayerId());
	GetWorld()->GetTimerManager().SetTimer(PlayerNameTimerHandle, this, &ADPPlayerState::SetPlayerNameDelayed, 0.5f, false);
	
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

	PlayerScoreData->TestBroadcast();
}

void ADPPlayerState::SetPlayerNameDelayed()
{
	PlayerScoreData->SetPlayerName(GetPlayerName());
}

void ADPPlayerState::SetSessionName()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		SessionName = GameInstance->GetSessionName();
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

	InitializePlayerState();
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
