#include "JudgePlayerController.h"

#include "DPPlayerController.h"
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

void AJudgePlayerController::InitConstUI()
{
	AJudgeGameState* GM = Cast<AJudgeGameState>(GetWorld()->GetGameState());
	checkf(GM, TEXT("Failed to get JudgeGameState"))
	TArray<TObjectPtr<APlayerState>> PA = GM->PlayerArray;
	checkf(PA.Num() <= 4, TEXT("The number of players is more than 4"))
	FNetLogger::LogInfo(TEXT("PA.Num(): %d"), PA.Num());
	for (int i = 0; i < PA.Num(); i++)
	{
		JudgeLevelUI->SetBlockContent(ETextBlockType::Id, i, PA[i]->GetPlayerName());
		JudgeLevelUI->SetBlockContent(ETextBlockType::Score, i, FString::FromInt(PA[i]->GetScore()));
		SetOccupationeName_Implementation(i, TEXT("BackSoo"));
	}
}

void AJudgePlayerController::NotifyTimerEnd_Implementation()
{
	// TODO: 내가 선정한거로 변경해야한다.
	ReturnVote_Implementation(EOccupation::ARCHAEOLOGIST);
}

void AJudgePlayerController::SetOccupationeName_Implementation(int index, const FString& Name)
{
	checkf(JudgeLevelUI, TEXT("JudgeLevelUI is nullptr"))
	JudgeLevelUI->SetBlockContent(ETextBlockType::Occupation, index, Name);
}

void AJudgePlayerController::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	IChatGameMode* GM = GetWorld()->GetAuthGameMode<IChatGameMode>();
	check(GM && GM->GetChatManager())
	GM->GetChatManager()->BroadcastChatMessage(SenderName, Message);
}

void AJudgePlayerController::ReturnVote_Implementation(EOccupation Type)
{
	AJudgeGameMode* GM = Cast<AJudgeGameMode>(GetWorld()->GetAuthGameMode());
	GM->SetVote(Type);
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

		FTimerHandle UIUpdateTimerHandle;
		GetWorldTimerManager().SetTimer(UIUpdateTimerHandle, this, &AJudgePlayerController::InitConstUI, 1.0f, false);
		bShowMouseCursor = true;
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
