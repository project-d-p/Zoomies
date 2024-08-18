#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetComponent.h"
#include "ChatUI.generated.h"

USTRUCT()
struct FChatUiInitializer
{
	GENERATED_BODY();

	UPROPERTY()
	UWorld* InWorld;
	UPROPERTY()
	UScrollBox* ChatScrollBox;
	UPROPERTY()
	UVerticalBox* ChatLogBox;
	UPROPERTY()
	UEditableTextBox* ChatEditableTextBox;
};

UCLASS()
class PROJ_A_API UChatUI : public UWidgetComponent
{
	GENERATED_BODY()

public:	
	UChatUI();

protected:
	virtual void BeginPlay() override;

public:
	void InitializeChatBox(FChatUiInitializer& Initializer);
	UFUNCTION()
	void OnChatBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void AddChatMessage(const FString& SenderName, const FString& Message);

	UFUNCTION(BlueprintCallable, Category="Chat")
	void ShowChat();
private:
	UPROPERTY()
	UWorld* World = nullptr;

	UPROPERTY()
	UEditableTextBox* ChatBox = nullptr;
	UPROPERTY()
	UScrollBox* ChatScrollBox = nullptr;
	UPROPERTY()
	UVerticalBox* ChatLogBox = nullptr;
	FInputModeDataBase* PreviousInputMode; 
};