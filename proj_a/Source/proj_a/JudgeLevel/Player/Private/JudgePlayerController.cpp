#include "JudgePlayerController.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "IChatGameMode.h"
#include "JudgeGameMode.h"
#include "JudgeGameState.h"
#include "JudgeLevelUI.h"
#include "JudgePlayerState.h"
#include "NetworkMessage.h"
#include "PathManager.h"
#include "ServerChatManager.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"

AJudgePlayerController::AJudgePlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder
	(PathManager::GetWidgetPath(EWidget::JUDGE_LEVEL));
	
	if (WidgetClassFinder.Succeeded())
	{
		JudgeLevelUI_BP = WidgetClassFinder.Class;
	}
}

void AJudgePlayerController::SetOccupationeName_Implementation(int index, const FString& Name)
{
	// XXX: If the client experiences slow loading times, issues may arise with the code below.
	if (JudgeLevelUI)
		return ;
	// check(JudgeLevelUI)
	JudgeLevelUI->SetBlockContent(ETextBlockType::Occupation, index, Name);
}

void AJudgePlayerController::InitializeUI_Implementation(const FUIInitData UIData)
{
	if (JudgeLevelUI)
	{
		for (int32 i = 0; i < UIData.PlayerData.Num(); i++)
		{
			const FPlayerInitData& PlayerData = UIData.PlayerData[i];
    
			JudgeLevelUI->SetBlockContent(ETextBlockType::Id, i, PlayerData.PlayerName);
			JudgeLevelUI->SetBlockContent(ETextBlockType::Score, i, FString::FromInt(PlayerData.Score));
			JudgeLevelUI->SetBlockContent(ETextBlockType::Occupation, i, PlayerData.Occupation);
		}
		JudgeLevelUI->SetVoterName(UIData.VoterName);
	}
	GetWorldTimerManager().ClearTimer(TH);
}

void AJudgePlayerController::SetVoterName_Implementation(const FString& Name)
{
	if (JudgeLevelUI)
	{
		JudgeLevelUI->SetVoterName(Name);
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
