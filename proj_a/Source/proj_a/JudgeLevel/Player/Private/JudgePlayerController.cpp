#include "JudgePlayerController.h"

#include "IChatGameMode.h"
#include "JudgeLevelUI.h"
#include "PathManager.h"
#include "ServerChatManager.h"
#include "Blueprint/UserWidget.h"

AJudgePlayerController::AJudgePlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder
	(PathManager::GetWidgetPath(EWidget::JUDGE_LEVEL));
	
	if (WidgetClassFinder.Succeeded())
	{
		JudgeLevelUI_BP = WidgetClassFinder.Class;
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
		JudgeLevelUI->SetBlockContent(ETextBlockType::Id, 0, TEXT("Hans"));
	}
}
