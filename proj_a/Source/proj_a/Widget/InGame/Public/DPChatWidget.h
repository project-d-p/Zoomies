// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatUI.h"
#include "DPChatWidget.generated.h"

/**
 * In-game chat widget
 */
UCLASS()
class PROJ_A_API UDPChatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadOnly)
	UChatUI* ChatUI = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* chatEditableTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UScrollBox* chatScrollBox;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ChatLogBox;
};
