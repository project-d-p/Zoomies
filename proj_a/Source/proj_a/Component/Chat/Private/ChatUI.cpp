#include "ChatUI.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableTextBox.h"

UChatUI::UChatUI()
	: ChatBox(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChatUI::BeginPlay()
{
	Super::BeginPlay();
}

void UChatUI::InitializeChatBox(UEditableTextBox* InChatBox)
{
	if (InChatBox)
	{
		ChatBox = InChatBox;
		ChatBox->SetVisibility(ESlateVisibility::Visible);
		ChatBox->SetIsEnabled(true);
		ChatBox->SetKeyboardFocus();
		ChatBox->OnTextCommitted.AddDynamic(this, &UChatUI::OnChatBoxCommitted);
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("ChatBox is nullptr"));
	}
}

void UChatUI::OnChatBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!Text.IsEmpty())
		{
			FString Message = Text.ToString();

			ChatBox->SetText(FText::GetEmpty());

			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				ADPPlayerController* DPPlayerController = Cast<ADPPlayerController>(PlayerController);
				if (DPPlayerController)
				{
					DPPlayerController->ServerSendChatMessage(Message);
				}

				FInputModeGameOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = false;
			}
		}
	}
}

void UChatUI::ShowChat()
{
	FNetLogger::EditerLog(FColor::Purple, TEXT("ShowChat"));
	if (ChatBox)
	{
		ChatBox->SetVisibility(ESlateVisibility::Visible);

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(ChatBox->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
		}
	}
}
