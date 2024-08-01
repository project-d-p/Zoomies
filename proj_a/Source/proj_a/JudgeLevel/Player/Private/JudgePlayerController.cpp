#include "JudgePlayerController.h"

#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "IChatGameMode.h"
#include "JudgeGameMode.h"
#include "JudgeGameState.h"
#include "JudgeLevelUI.h"
#include "JudgePlayerState.h"
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
	// TODO: Address this after alpha testing.
	check(JudgeLevelUI)
	JudgeLevelUI->SetBlockContent(ETextBlockType::Occupation, index, Name);
}

void AJudgePlayerController::InitializeUI_Implementation(const FUIInitData UIData)
{
	check(JudgeLevelUI)
	for (int32 i = 0; i < UIData.PlayerData.Num(); i++)
	{
		const FPlayerInitData& PlayerData = UIData.PlayerData[i];
    
		JudgeLevelUI->SetBlockContent(ETextBlockType::Id, i, PlayerData.PlayerName);
		JudgeLevelUI->SetBlockContent(ETextBlockType::Score, i, FString::FromInt(PlayerData.Score));
		JudgeLevelUI->SetBlockContent(ETextBlockType::Occupation, i, PlayerData.Occupation);
	}
	JudgeLevelUI->SetVoterName(UIData.VoterName);
}

void AJudgePlayerController::SetVoterName_Implementation(const FString& Name)
{
	check(JudgeLevelUI)
	JudgeLevelUI->SetVoterName(Name);
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
	check(GM)
	GM->AddVote(Type);
}

void AJudgePlayerController::RequestUIData_Implementation()
{
	AJudgeGameMode* GM = Cast<AJudgeGameMode>(GetWorld()->GetAuthGameMode());
	check(GM)
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AJudgePlayerController* JPS = Cast<AJudgePlayerController>(*It);
		check(JPS)
		JPS->InitializeUI(GM->GetUiData());
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

		FTimerHandle TH;
		GetWorldTimerManager().SetTimer(TH, this, &AJudgePlayerController::RequestUIData, 1.f, false);
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
	GS->SetScore(MainPC->GetPrivateScoreManagerComponent()->GetPrivatePlayerScore());

	GS->SetCapturedAnimals(MainPC->GetPrivateScoreManagerComponent()->GetCapturedAnimals());
	GS->SetScoreDatas(MainPC->GetPrivateScoreManagerComponent()->GetScoreDatas());
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
	NGS->SetFinalScoreData(GS->GetFinalScoreData());
}
