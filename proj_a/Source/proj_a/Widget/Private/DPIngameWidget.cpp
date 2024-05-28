// Fill out your copyright notice in the Description page of Project Settings.


#include "DPIngameWidget.h"

#include "DPPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UDPIngameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatBox)
	{
		ChatBox->SetVisibility(ESlateVisibility::Visible);
		ChatBox->SetIsEnabled(true);
		ChatBox->SetKeyboardFocus();
		ChatBox->OnTextCommitted.AddDynamic(this, &UDPIngameWidget::OnChatBoxCommitted);
	}
}

void UDPIngameWidget::OnChatBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
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

void UDPIngameWidget::ShowChat()
{
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
