#include "ChatUI.h"
#include "DPPlayerController.h"
#include "IChatGameMode.h"
#include "IChatGameState.h"
#include "JudgePlayerController.h"
#include "Fonts/FontTypes.h"
#include "ServerChatManager.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameModeBase.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"

UChatUI::UChatUI()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChatUI::BeginPlay()
{
	Super::BeginPlay();
}

void UChatUI::InitializeChatBox(FChatUiInitializer& Initializer)
{
	if (Initializer.InWorld)
	{
		World = Initializer.InWorld;

		IChatGameState* GS = Cast<IChatGameState>(World->GetGameState());
		check(GS && GS->GetChatManager())
		GS->GetChatManager()->setChatUI(this);
	}
	if (Initializer.ChatEditableTextBox)
	{
		ChatBox = Initializer.ChatEditableTextBox;
		ChatBox->SetVisibility(ESlateVisibility::Visible);
		ChatBox->SetIsEnabled(true);
		ChatBox->SetKeyboardFocus();
		ChatBox->OnTextCommitted.AddDynamic(this, &UChatUI::OnChatBoxCommitted);
	}
	if (Initializer.ChatLogBox)
	{
		ChatLogBox = Initializer.ChatLogBox;
	}
	if (Initializer.ChatScrollBox)
	{
		ChatScrollBox = Initializer.ChatScrollBox;
	}
}

void UChatUI::OnChatBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		check(World);
		APlayerController* PC = Cast<APlayerController>(World->GetFirstPlayerController());
		// bool bOldShowMouseCursor = PC->bShowMouseCursor;
		check(PC)
		if (!Text.IsEmpty())
		{
			FString Message = Text.ToString();
			
			ChatBox->SetText(FText::GetEmpty());
			FString SenderName = PC->PlayerState->GetPlayerName();
			IChatGameMode* GM = Cast<IChatGameMode>(World->GetAuthGameMode());
			if (GM)
			{
				check(GM->GetChatManager())
				GM->GetChatManager()->BroadcastChatMessage(SenderName, Message);
			}
			else
			{
				// TODO: Refactor needed - current implementation is suboptimal
				if (Cast<ADPPlayerController>(PC))
				{
					ADPPlayerController* DP = Cast<ADPPlayerController>(PC);
					DP->ServerSendChatMessage(SenderName, Message);
				}
				else if (Cast<AJudgePlayerController>(PC))
				{
					AJudgePlayerController* Judge = Cast<AJudgePlayerController>(PC);
					Judge->ServerSendChatMessage(SenderName, Message);
				}
			}
		}
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			ChatBox->SetKeyboardFocus();
		}), 0.1f, false);
		// DPPlayerController->bShowMouseCursor = false;
	}
}

void UChatUI::AddChatMessage(const FString& SenderName, const FString& Message)
{
	check(ChatLogBox)
	UTextBlock* ChatMessage = NewObject<UTextBlock>(ChatLogBox);

	check(ChatMessage)
	FString FullMessage = FString::Printf(TEXT("%s: %s"), *SenderName, *Message);
	ChatMessage->SetText(FText::FromString(FullMessage));
	UVerticalBoxSlot* NewSlot = ChatLogBox->AddChildToVerticalBox(ChatMessage);
	NewSlot->SetPadding(FMargin(5.0f, 5.0f));
	
	FSlateFontInfo FontInfo;
	FontInfo.Size = 28.0f;
	UObject* FontObj = LoadObject<UObject>(this, TEXT("/Script/Engine.Font'/Game/font/HakgyoansimKkokkomaR_Font.HakgyoansimKkokkomaR_Font'"), NULL, LOAD_None, NULL);
	check(FontObj)
	FontInfo.FontObject = FontObj;
	FontInfo.TypefaceFontName = "SemiBold";
	ChatMessage->SetFont(FontInfo);
	
	check(ChatScrollBox)
	ChatScrollBox->ScrollToEnd();
}

void UChatUI::ShowChat()
{
	check(World)

	APlayerController* PlayerController = World->GetFirstPlayerController();
	check(PlayerController)
	
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ChatBox->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
}