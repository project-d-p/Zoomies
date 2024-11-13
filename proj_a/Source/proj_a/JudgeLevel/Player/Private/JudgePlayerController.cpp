#include "JudgePlayerController.h"

#include "DPPlayerController.h"
#include "EngineUtils.h"
#include "IChatGameMode.h"
#include "JudgeGameMode.h"
#include "JudgeGameState.h"
#include "JudgeLevelUI.h"
#include "JudgePlayerState.h"
#include "NetworkMessage.h"
#include "PathManager.h"
#include "ServerChatManager.h"
#include "Blueprint/UserWidget.h"
#include "JudgeLevelComponent.h"
#include "JudgeInputComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AJudgePlayerController::AJudgePlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder
	(PathManager::GetWidgetPath(EWidget::JUDGE_LEVEL));
	
	if (WidgetClassFinder.Succeeded())
	{
		JudgeLevelUI_BP = WidgetClassFinder.Class;
	}
	TextureTransferManager = CreateDefaultSubobject<UTextureTransferManager>(TEXT("TextureTransferManager"));
	TextureTransferManager->OnDataTransferComplete.BindDynamic(TextureTransferManager, &UTextureTransferManager::OnTextureTransferComplete);
	static ConstructorHelpers::FObjectFinder<USoundWave> SoundAsset(TEXT("/Game/sounds/effect/Turn/Chin.Chin"));
	if (SoundAsset.Succeeded())
	{
		TurnStartSound = SoundAsset.Object;
	}
	
	LevelComponent = CreateDefaultSubobject<UJudgeLevelComponent>(TEXT("ULC_JudgeLevel"));
}

void AJudgePlayerController::SetOccupationeName_Implementation(int index, const FString& Name)
{
	// XXX: If the client experiences slow loading times, issues may arise with the code below.
	if (JudgeLevelUI)
		return ;
}

void AJudgePlayerController::InitializeUI_Implementation(const FUIInitData UIData)
{
	if (JudgeLevelUI)
	{
		JudgeLevelUI->SetVoterName(UIData.VoterName);
		AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
		check(GS)
		GS->CurrentVotedPlayerName = UIData.VoterName;
		if (TurnStartSound)
		{
			float VolumeMultiplier = 0.2f;
		
			UGameplayStatics::PlaySound2D(this, TurnStartSound, VolumeMultiplier);
		}
	}
	GetWorldTimerManager().ClearTimer(TH);
}



void AJudgePlayerController::SetVoterName_Implementation(const FString& Name)
{
	if (JudgeLevelUI)
	{
		JudgeLevelUI->SetVoterName(Name);
		if (TurnStartSound) 
		{
			UGameplayStatics::PlaySound2D(this, TurnStartSound);
		}
	}
}

void AJudgePlayerController::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	IChatGameMode* GM = GetWorld()->GetAuthGameMode<IChatGameMode>();
	check(GM && GM->GetChatManager())
	GM->GetChatManager()->BroadcastChatMessage(SenderName, Message);
}

void AJudgePlayerController::ReturnVote_Implementation(EPlayerJob Type)
{
	AJudgeGameMode* GM = Cast<AJudgeGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->AddVote(Type);
	}
}

void AJudgePlayerController::RequestUIData_Implementation()
{
	AJudgeGameMode* GM = Cast<AJudgeGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM)
		return ;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AJudgePlayerController* JPC = Cast<AJudgePlayerController>(*It);
		if (!JPC)
			continue;
		FUIInitData Data = GM->GetUiData();
		if (Data.bInitSuccessful)
		{
			JPC->InitializeUI(Data);
		}
	}
}

void AJudgePlayerController::RequestCharacter_Implementation()
{
	if (this->GetCharacter() != nullptr)
	{
		return;
	}
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADynamicTexturedCharacter::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		ADynamicTexturedCharacter* FoundCharacter = Cast<ADynamicTexturedCharacter>(Actor);
		if (FoundCharacter)
		{
			if (FoundCharacter->bPlayerAssigned == false)
			{
				Possess(FoundCharacter);
				FoundCharacter->bPlayerAssigned = true;
				break;
			}
		}
	}
}

void AJudgePlayerController::OnPossessEvent(APawn* /*OldPawn*/, APawn* /*NewPawn*/)
{
	GetWorldTimerManager().ClearTimer(CTH);
}

void AJudgePlayerController::BeginPlay()
{
	Super::BeginPlay();
	bAutoManageActiveCameraTarget = false;

	if (IsLocalController())
	{
		checkf(JudgeLevelUI_BP, TEXT("JudgeLevelUI Widget is nullptr"));
		JudgeLevelUI = CreateWidget<UJudgeLevelUI>(this, JudgeLevelUI_BP);
		checkf(JudgeLevelUI, TEXT("Failed to create JudgeLevelUI"));
		JudgeLevelUI->AddToViewport();
		IsBeginPlay = true;
		bShowMouseCursor = true;
		
		GetWorldTimerManager().SetTimer(TH, this, &AJudgePlayerController::RequestUIData, 1.f, true);
		findMyCamera();
		RequestCharacter();
		GetWorldTimerManager().SetTimer(CTH, this, &AJudgePlayerController::RequestCharacter, 1.f, true);
		/*
		 * This Delegate will Called when the Controller Possess New Pawn or UnPossess Old Pawn on both Server and Client Side
		 * Look at the Possess() function in APlayerController Class
		 * Pawn is Set as Replicated so OnRep_Pawn() will be called on both Server and Client Side
		 * OnRep_Pawn() will call OnPossessedPawnChanged Delegate to Broadcast the Possess Event
		*/
		OnPossessedPawnChanged.AddDynamic(this, &AJudgePlayerController::OnPossessEvent);
	}
	SetInputMode(FInputModeGameAndUI());
	ActivateCurrentComponent(this);
}

void AJudgePlayerController::SeamlessTravelFrom(APlayerController* OldPC)
{
	Super::SeamlessTravelFrom(OldPC);
	
	ADPPlayerController* MainPC = Cast<ADPPlayerController>(OldPC);
	checkf(MainPC, TEXT("OldPC is not ADPPlayerController"))
	AJudgePlayerState* GS = GetPlayerState<AJudgePlayerState>();
	checkf(GS, TEXT("Failed to get JudgePlayerState"))
	checkf(MainPC->GetPrivateScoreManagerComponent(), TEXT("Failed to get PrivateScoreManagerComponent"))
}

void AJudgePlayerController::SeamlessTravelTo(APlayerController* NewPC)
{
	Super::SeamlessTravelTo(NewPC);
}

void AJudgePlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	// for (TActorIterator<ADynamicTexturedCharacter> It(GetWorld()); It; ++It)
	// {
	// 	if (It->bPlayerAssigned)
	// 		continue;
	// 	It->SetOwner(this);
	// 	It->bPlayerAssigned = true;
	// 	break;
	// }
}

void AJudgePlayerController::GetSeamlessTravelActorList(bool bToTransitionMap, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransitionMap, ActorList);

	if (!GetWorld()->GetMapName().Contains("judgeLevel"))
	{
		return ;
	}
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	check(GameInstance)
	GameInstance->network_failure_manager_->TryReset();
	UDataManager* DataManager = GameInstance->network_failure_manager_->GetDataManager();
	check(DataManager)
	DataManager->ClearSeamlessDataArray();
	AJudgeGameState* GameState = GetWorld()->GetGameState<AJudgeGameState>();
	check(GameState)

	for (auto PlayerState_ : GameState->PlayerArray)
	{
		AJudgePlayerState* JudgePlayerState = Cast<AJudgePlayerState>(PlayerState_);
		if (JudgePlayerState)
		{
			UPlayerScoreData* NewData = NewObject<UPlayerScoreData>(DataManager);
			if (NewData)
			{
				UPlayerScoreData* PlayerScoreData = JudgePlayerState->GetPlayerScoreData();
				NewData->InitializeData();
				NewData->SetPlayerName(PlayerScoreData->GetPlayerName());
				NewData->SetPlayerId(PlayerScoreData->GetPlayerId());
				NewData->SetPlayerJob(PlayerScoreData->GetPlayerJob());
				NewData->SetScore(PlayerScoreData->GetScore());
				DataManager->AddSeamlessDataToArray(TEXT("PlayerScoreSeamless"), NewData);
			}
		}
	}
}

void AJudgePlayerController::ShowUI_ESC()
{
	if (JudgeLevelUI == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("JudgeLevelUI is nullptr"));
		return;
	}
	UUserWidget* WidgetESC = Cast<UUserWidget>(JudgeLevelUI->GetWidgetFromName("WBP_Esc_Menu"));
	if (WidgetESC)
	{
		ESlateVisibility CurrentVisibility = WidgetESC->GetVisibility();
		if (CurrentVisibility == ESlateVisibility::Visible)
		{
			WidgetESC->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			WidgetESC->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DPPlayerController:: UIWidget->GetWidgetFromName(ESC) is nullptr"));
	}
}

void AJudgePlayerController::ActivateCurrentComponent(AJudgePlayerController* LocalPlayerController)
{
	if (LevelComponent)
	{
		LevelComponent->PrimaryComponentTick.bCanEverTick = true;
		LevelComponent->Activate(true);
		LevelComponent->SetComponentTickEnabled(true);
		LevelComponent->RegisterComponent();
		if (LocalPlayerController)
		{
			LevelComponent->Set_PC(LocalPlayerController);
			if (!LocalPlayerController->IsLocalController())
			{
				UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LocalPlayerController is not local controller"));
				return ;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LocalPlayerController is nullptr"));
			return ;
		}

		if (UJudgeInputComponent* IC_Local = LevelComponent->GetInputComponent())
		{
			IC_Local->Activate(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::InputComponent is nullptr"));
		}
	}
}

void AJudgePlayerController::findMyCamera()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

	AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
	if (!GS)
	{
		return;
	}
	if (GS->GS_PlayerData.Num() == 0)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			findMyCamera();
		}), 0.1f, false);
		return;
	}
	
	AJudgePlayerState* PS = Cast<AJudgePlayerState>(PlayerState);
	if (!PS)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			findMyCamera();
		}), 0.1f, false);
		return;
	}

	int32 PlayerId = PS->GetPlayerId();
	int32 CameraIndex = -1;

	for (const FPlayerInitData& Data : GS->GS_PlayerData)
	{
		if (Data.PlayerId == PlayerId)
		{
			CameraIndex = Data.CameraIndex;
			break;
		}
	}
	
	if (CameraIndex == -1)
	{
		return;
	}

	for (AActor* Actor : FoundActors)
	{
		FString ActorName = Actor->GetActorLabel();
		if (ActorName == FString::Printf(TEXT("CameraActor_%d"), CameraIndex))
		{
			CameraActor = Actor;
			SetViewTarget(Actor);
			break;
		}
	}
}