#include "JudgePlayerController.h"

#include "DPPlayerController.h"
#include "IChatGameMode.h"
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

AJudgePlayerController::~AJudgePlayerController()
{
	if (DES)
		delete DES;
}

void AJudgePlayerController::InitConstUI_Implementation(const TArray<FString>& Ids, const TArray<int32>& Scores)
{
	constexpr int32 PLAYER_NUMBER = 4;
	checkf(Ids.Num() == PLAYER_NUMBER && Scores.Num() == PLAYER_NUMBER, TEXT("Invalid Ids or Scores"))
	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		JudgeLevelUI->SetBlockContent(ETextBlockType::Id, i, Ids[i]);
		JudgeLevelUI->SetBlockContent(ETextBlockType::Score, i, FString::FromInt(Scores[i]));
	}
}

void AJudgePlayerController::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	IChatGameMode* GM = GetWorld()->GetAuthGameMode<IChatGameMode>();
	check(GM && GM->GetChatManager())
	GM->GetChatManager()->BroadcastChatMessage(SenderName, Message);
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
}
