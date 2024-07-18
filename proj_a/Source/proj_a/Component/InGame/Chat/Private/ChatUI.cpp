#include "ChatUI.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
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

		ADPPlayerController* PC = Cast<ADPPlayerController>(World->GetFirstPlayerController());
		check(PC && PC->GetChatManager())
		PC->GetChatManager()->setChatUI(this);
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
		ADPPlayerController* DPPlayerController = Cast<ADPPlayerController>(World->GetFirstPlayerController());
		check(DPPlayerController)
		if (!Text.IsEmpty())
		{
			FString Message = Text.ToString();
			
			ChatBox->SetText(FText::GetEmpty());
			DPPlayerController->SendChatMessageToServer(Message);
		}
		FInputModeGameOnly InputMode;
		DPPlayerController->SetInputMode(InputMode);
		DPPlayerController->bShowMouseCursor = false;
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