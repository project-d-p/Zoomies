#include "ResultLevelGameState.h"

#include <algorithm>

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "EngineUtils.h"
#include "EnumTypes.h"
#include "PathManager.h"
#include "ResultWidgetActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AResultLevelGameState::AResultLevelGameState()
{
	bReplicates = true;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> RESULT_WIDGET
	(PathManager::GetWidgetPath(EWidget::RESULT_WIDGET));
	if (RESULT_WIDGET.Succeeded())
	{
		CalculateWidget = RESULT_WIDGET.Class;
	}
	
	static ConstructorHelpers::FClassFinder<UUserWidget> CALCULATE_WIDGET
	(PathManager::GetWidgetPath(EWidget::CALCULATE_WIDGET));
	if (CALCULATE_WIDGET.Succeeded())
	{
		ResultWidget = CALCULATE_WIDGET.Class;
	}
	
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
}

bool AResultLevelGameState::IsAlreadySet(const FFinalScoreData& FinalScoreData)
{
	return std::any_of(FinalScoreDataArray.begin(), FinalScoreDataArray.end(),
		[&FinalScoreData](const FFinalScoreData& ExistingData)
		{
			return ExistingData.PlayerName == FinalScoreData.PlayerName; // 조건
		});
}

void AResultLevelGameState::AddLeftPlayers()
{
	TArray<FFinalScoreData> LeftPlayers;
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		UDataManager* DataManager = GameInstance->network_failure_manager_->GetDataManager();
		if (DataManager)
		{
			UDataArray* PlayerDataArray = DataManager->GetSeamlessDataArray(TEXT("PlayerScoreSeamless"));
			if (PlayerDataArray)
			{
				for (UBaseData* Data : PlayerDataArray->DataArray)
				{
					UPlayerScoreData* PlayerScoreData = Cast<UPlayerScoreData>(Data);
					FFinalScoreData FinalScoreData;
					FinalScoreData = PlayerScoreData->GetScore();
					if (PlayerScoreData && !IsAlreadySet(FinalScoreData))
					{
						FinalScoreData.bIsLeft = true;
						LeftPlayers.Add(FinalScoreData);
					}
				}
			}
		}
	}

	LeftPlayers.Sort([](const FFinalScoreData& A, const FFinalScoreData& B) {
		float ScoreA = A.bIsDetected ? A.PublicTotalScore : A.PrivateTotalScore;
		float ScoreB = B.bIsDetected ? B.PublicTotalScore : B.PrivateTotalScore;
		return ScoreA > ScoreB;
	});

	FinalScoreDataArray.Append(LeftPlayers);

	if (FinalScoreDataArray.Num() < 4)
	{
		for (int i = FinalScoreDataArray.Num(); i < 4; i++)
		{
			FFinalScoreData FinalScoreData;
			FinalScoreData.PlayerName = TEXT("NONE");
			FinalScoreDataArray.Add(FinalScoreData);
		}
	}
}

void AResultLevelGameState::NotifyPlayersAllTraveled()
{
	this->SetPlayerScores();
	this->SetMyRank();
	this->AddLeftPlayers();

	NotifyAllScoresCalculated(FinalScoreDataArray);
}

void AResultLevelGameState::NotifyAllScoresCalculated_Implementation(
	const TArray<FFinalScoreData>& InFinalScoreDataArray)
{
	/* Create Widget */
	CalculateWidgetInstance = CreateWidget<UDPCalculateWidget>(GetWorld(), ResultWidget);
	if (CalculateWidgetInstance)
	{
		CalculateWidgetInstance->AddToViewport();
		CalculateWidgetInstance->OnScoresUpdated(InFinalScoreDataArray);
	}
}

void AResultLevelGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void AResultLevelGameState::SetMyRank()
{
	FinalScoreDataArray.Sort([](const FFinalScoreData& A, const FFinalScoreData& B) {
		float ScoreA = A.bIsDetected ? A.PublicTotalScore : A.PrivateTotalScore;
		float ScoreB = B.bIsDetected ? B.PublicTotalScore : B.PrivateTotalScore;
		return ScoreA > ScoreB;
	});
}

void AResultLevelGameState::SetPlayerScores()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADPPlayerController* PlayerController = Cast<ADPPlayerController>(*Iterator);
		if (!PlayerController)
		{
			continue ;
		}
		ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
		if (!PlayerState)
		{
			continue ;
		}
		FFinalScoreData FinalScoreData;
		FinalScoreData = PlayerState->GetPlayerScoreData()->GetScore();
		FinalScoreDataArray.Add(FinalScoreData);
	}
}

void AResultLevelGameState::BeginPlay()
{
	Super::BeginPlay();

	ADPCharacter* Character = Cast<ADPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Character)
	{
		Character->SetReplicatingMovement(true);
	}
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		if (!HasAuthority())
		{
			OnHostMigrationDelegate = GameInstance->network_failure_manager_->OnHostMigration().AddUObject(this, &AResultLevelGameState::OnHostMigration);
		}
		GameInstance->network_failure_manager_->ClearWidget();
	}
}

void AResultLevelGameState::OnHostMigration(UWorld* World, UDataManager* DataManager)
{
	// TODO: Result Level에서 저장되어야 할 것
	// 1. 플레이어들의 최종 점수 : PlayerState에서 어차피 저장되지 않나?
}

void AResultLevelGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// ���� �ν��Ͻ��� ���� ���� ����
    UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
    if (GameInstance)
    {
    	IOnlineSessionPtr SessionInt = GameInstance->GetOnlineSessionInterface();
    	if (SessionInt.IsValid())
    	{
    		SessionInt->DestroySession(GameInstance->GetSessionName());
    	}
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
    }
}

void AResultLevelGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AResultLevelGameState::SetWinner()
{
	FString Winner = FinalScoreDataArray[0].PlayerName;
	UE_LOG(LogTemp, Warning, TEXT("Winner : %s"), *Winner);

	FString MyName = GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName();

	for (TActorIterator<ADPCharacter> It(GetWorld()); It; ++It)
	{
		ADPCharacter* Character = *It;
		if (Character)
		{
			if (Character->GetPlayerState()->GetPlayerName() == Winner)
			{
				Character->SetCrownMesh();
			}
		}
	}
}