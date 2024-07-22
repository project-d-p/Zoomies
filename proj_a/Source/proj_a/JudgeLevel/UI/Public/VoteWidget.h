// VoteWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "VoteWidget.generated.h"

UCLASS()
class PROJ_A_API UVoteWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* Player1Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Player2Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Player3Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Player4Button;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* Player1CanvasPanel;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* Player2CanvasPanel;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* Player3CanvasPanel;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* Player4CanvasPanel;

    UPROPERTY(meta = (BindWidget))
    UImage* Player1OccupationImage;

    UPROPERTY(meta = (BindWidget))
    UImage* Player2OccupationImage;

    UPROPERTY(meta = (BindWidget))
    UImage* Player3OccupationImage;

    UPROPERTY(meta = (BindWidget))
    UImage* Player4OccupationImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Player1NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Player2NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Player3NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Player4NameText;

    UFUNCTION()
    void OnPlayer1ButtonClicked();

    UFUNCTION()
    void OnPlayer2ButtonClicked();

    UFUNCTION()
    void OnPlayer3ButtonClicked();

    UFUNCTION()
    void OnPlayer4ButtonClicked();

    void SetPlayerOccupation(int32 PlayerIndex, const FString& OccupationName, UTexture2D* OccupationImage);

private:
    TArray<FString> OccupationNames;
    TArray<UTexture2D*> OccupationImages;
    
    void InitializeOccupations();
};