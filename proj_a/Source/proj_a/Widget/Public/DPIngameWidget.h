// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "DPIngameWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API UDPIngameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// UFUNCTION()
	// void OnChatBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	// UFUNCTION(BlueprintCallable, Category="Chat")
	// void ShowChat();
	// virtual void NativeConstruct() override;
private:
	// UPROPERTY(meta = (BindWidget))
	// UEditableTextBox* ChatBox;
};
