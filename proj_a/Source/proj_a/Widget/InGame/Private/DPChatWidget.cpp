// Fill out your copyright notice in the Description page of Project Settings.


#include "DPChatWidget.h"

void UDPChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ChatUI = NewObject<UChatUI>(this);
	if (ChatUI)
	{
		FChatUiInitializer ChatUiInitializer;

		ChatUiInitializer.ChatEditableTextBox = chatEditableTextBox;
		ChatUiInitializer.ChatScrollBox = chatScrollBox;
		ChatUiInitializer.ChatLogBox = ChatLogBox;
		ChatUiInitializer.InWorld = GetWorld();
		ChatUI->InitializeChatBox(ChatUiInitializer);
	}
}
