#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "DPResultWidget.generated.h"

UCLASS()
class UDPResultWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetResultText(const FString& Text);

	UFUNCTION(BlueprintImplementableEvent)
	void OnResultTextSet();

	UFUNCTION(BlueprintCallable, Category = "End Game")
	void ShowEndGame();
	
protected:
	void InitSingleTextBlock(UHorizontalBox* HorizontalBox, UTextBlock* TextBlock);
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* PlayerNameContainer;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* PlayerRankContainer;

	UPROPERTY()
	UTextBlock* PlayerNameText;
	UPROPERTY()
	UTextBlock* PlayerRankText;
};
