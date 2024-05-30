#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/EditableTextBox.h"
#include "ChatUI.generated.h"

UCLASS()
class PROJ_A_API UChatUI : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChatUI();

protected:
	virtual void BeginPlay() override;

public:	
	void InitializeChatBox(UEditableTextBox* InChatBox);

	UFUNCTION()
	void OnChatBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void ShowChat();
	
private:
	UEditableTextBox* ChatBox;
};