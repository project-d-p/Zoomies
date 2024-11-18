#include "DPInGameState.h"

#include "DPJobAssign.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "TimeData.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

class UTimeData;

ADPInGameState::ADPInGameState()
{
	bReplicates = true;
	TimerManager = CreateDefaultSubobject<UClientTimerManager>(TEXT("TimerManager"));
	ScoreManager = CreateDefaultSubobject<UClientScoreMananger>(TEXT("ScoreManager"));
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/widget/widget_jobAssign.widget_jobAssign_C"));
	if (WidgetBPClass.Succeeded())
	{
		WidgetClass = WidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetLoadingClass(TEXT("/Game/widget/widget_loading.widget_loading_C"));
	if (WidgetLoadingClass.Succeeded())
	{
		WidgetLoading = WidgetLoadingClass.Class;
	}
}

void ADPInGameState::MulticastPlayerJob_Implementation() const
{
	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
		if (PlayerState)
		{
			if (JobWidgetInstance)
			{
				UDPJobAssign* JobWidget = Cast<UDPJobAssign>(JobWidgetInstance);
				check(JobWidget)
				JobWidget->AddToViewport();
				JobWidget->OnJobAssigned(PlayerState->GetPlayerJob());
			}
		}
	}
}

void ADPInGameState::BeginPlay()
{
	Super::BeginPlay();

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (!HasAuthority())
	{
		if (GameInstance)
		{
			OnHostMigrationDelegate = GameInstance->network_failure_manager_->OnHostMigration().AddUObject(this, &ADPInGameState::OnHostMigration);
		}
	}
	if (!JobWidgetInstance)
	{
		JobWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	}
	if (!LoadingWidgetInstance)
	{
		LoadingWidgetInstance = CreateWidget<UDPLoadingWidget>(GetWorld(), WidgetLoading);
		LoadingWidgetInstance->AddToViewport(99);
	}
}

void ADPInGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
}

void ADPInGameState::LevelAllReady_Implementation()
{
	if (LoadingWidgetInstance)
	{
		LoadingWidgetInstance->RemoveFromParent();
	}
	bAllReady = true;
}

void ADPInGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void ADPInGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPInGameState, TimerManager);
	DOREPLIFETIME(ADPInGameState, ScoreManager);
}

void ADPInGameState::OnHostMigration(UWorld* World, UDataManager* DataManager)
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
	UTimeData* TimeData = NewObject<UTimeData>(DataManager, UTimeData::StaticClass());
	if (TimeData)
	{
		TimeData->InitializeData();
		TimeData->SetTimeRemaining(TimerManager->TimeRemaining);
		DataManager->AddDataToSingle(TEXT("TimeData"), TimeData);
	}
}

// /Script/UMGEditor.WidgetBlueprint'/Game/widget/widget_loading.widget_loading'