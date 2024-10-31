#include "JudgePlayerController.h"

#include "DPPlayerController.h"
#include "EngineUtils.h"
#include "IChatGameMode.h"
#include "JudgeGameMode.h"
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
		if (TurnStartSound)  // 사운드가 유효한지 확인
		{
			// 2D 사운드를 재생 (UI에서 사용)
			UGameplayStatics::PlaySound2D(this, TurnStartSound);
			//log on screen
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TurnStartSound"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TurnStartSound is not valid"));
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

void AJudgePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		checkf(JudgeLevelUI_BP, TEXT("JudgeLevelUI Widget is nullptr"));
		JudgeLevelUI = CreateWidget<UJudgeLevelUI>(this, JudgeLevelUI_BP);
		checkf(JudgeLevelUI, TEXT("Failed to create JudgeLevelUI"));
		JudgeLevelUI->AddToViewport();
		IsBeginPlay = true;
		bShowMouseCursor = true;
		
		GetWorldTimerManager().SetTimer(TH, this, &AJudgePlayerController::RequestUIData, 1.f, true);
	}
	SetInputMode(FInputModeGameAndUI());
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

	ADPPlayerController* NPC = Cast<ADPPlayerController>(NewPC);
	check(NPC)
	ADPPlayerState* NGS = NPC->GetPlayerState<ADPPlayerState>();
	check(NGS)
	AJudgePlayerState* GS = GetPlayerState<AJudgePlayerState>();
	check(GS)
	NGS->SetPlayerScoreData(GS->GetPlayerScoreData());
}

void AJudgePlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	for (TActorIterator<ADynamicTexturedCharacter> It(GetWorld()); It; ++It)
	{
		if (It->bPlayerAssigned)
			continue;
		It->SetOwner(this);
		It->bPlayerAssigned = true;
		ActivateCurrentComponent(this);
		break;
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
