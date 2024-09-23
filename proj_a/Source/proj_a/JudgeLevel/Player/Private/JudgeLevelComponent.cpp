#include "JudgeLevelComponent.h"

#include "DPPlayerController.h"
#include "IChatGameMode.h"
#include "JudgeGameMode.h"
#include "LobbyInputComponent.h"
#include "ServerChatManager.h"
#include "PathManager.h"

UJudgeLevelComponent::UJudgeLevelComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder
	(PathManager::GetWidgetPath(EWidget::JUDGE_LEVEL));
	
	if (WidgetClassFinder.Succeeded())
	{
		JudgeLevelUI_BP = WidgetClassFinder.Class;
	}

	InputComponent = CreateDefaultSubobject<ULobbyInputComponent>(TEXT("InputComponent"));

	InputComponent->SetLevelComponent(this);
}

void UJudgeLevelComponent::InitializeUI_Implementation(const FUIInitData UIData)
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
	PC->GetWorldTimerManager().ClearTimer(TH);
}

void UJudgeLevelComponent::SetVoterName_Implementation(const FString& Name)
{
	if (JudgeLevelUI)
	{
		JudgeLevelUI->SetVoterName(Name);
	}
}

void UJudgeLevelComponent::SetOccupationeName_Implementation(int index, const FString& Name)
{
	// XXX: If the client experiences slow loading times, issues may arise with the code below.
	if (JudgeLevelUI)
		return ;
	// check(JudgeLevelUI)
	JudgeLevelUI->SetBlockContent(ETextBlockType::Occupation, index, Name);
}

void UJudgeLevelComponent::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	IChatGameMode* GM = GetWorld()->GetAuthGameMode<IChatGameMode>();
	check(GM && GM->GetChatManager())
	GM->GetChatManager()->BroadcastChatMessage(SenderName, Message);
}

void UJudgeLevelComponent::ReturnVote_Implementation(EPlayerJob Type)
{
	AJudgeGameMode* GM = Cast<AJudgeGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->AddVote(Type);
	}
}

void UJudgeLevelComponent::RequestUIData_Implementation()
{
	AJudgeGameMode* GM = Cast<AJudgeGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM)
		return ;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADPPlayerController* TPC = Cast<ADPPlayerController>(*It);
		if (!TPC)
			continue;
		FUIInitData Data = GM->GetUiData();
		if (Data.bInitSuccessful)
		{
			UJudgeLevelComponent* JLC = Cast<UJudgeLevelComponent>(TPC->GetLevelComponent());
			if (JLC)
				JLC->InitializeUI(Data);
		}
	}
}

void UJudgeLevelComponent::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<ADPPlayerController>(GetOwner());

	if (PC->IsLocalController())
	{
		checkf(JudgeLevelUI_BP, TEXT("JudgeLevelUI Widget is nullptr"));
		JudgeLevelUI = CreateWidget<UJudgeLevelUI>(PC, JudgeLevelUI_BP);
		checkf(JudgeLevelUI, TEXT("Failed to create JudgeLevelUI"));
		JudgeLevelUI->AddToViewport();
		IsBeginPlay = true;
		PC->bShowMouseCursor = true;
		
		PC->GetWorldTimerManager().SetTimer(TH, this, &UJudgeLevelComponent::RequestUIData, 1.f, true);
	}
}
