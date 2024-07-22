// VoteWidget.cpp
#include "VoteWidget.h"

void UVoteWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeOccupations();

    if (Player1Button) Player1Button->OnClicked.AddDynamic(this, &UVoteWidget::OnPlayer1ButtonClicked);
    if (Player2Button) Player2Button->OnClicked.AddDynamic(this, &UVoteWidget::OnPlayer2ButtonClicked);
    if (Player3Button) Player3Button->OnClicked.AddDynamic(this, &UVoteWidget::OnPlayer3ButtonClicked);
    if (Player4Button) Player4Button->OnClicked.AddDynamic(this, &UVoteWidget::OnPlayer4ButtonClicked);
}

void UVoteWidget::InitializeOccupations()
{
    OccupationNames = {"Archaeologist", "Poacher", "Environmentalist", "Ringmaster", "Terrorist", "Check", "Cross"};
    
    // Load occupation images
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, TEXT("/Game/image/character/pickaxe")));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, TEXT("/Game/image/character/gun")));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, TEXT("/Game/image/character/earth")));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, TEXT("/Game/image/character/circus")));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, TEXT("/Game/image/character/bomb")));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, TEXT("/Game/image/ui/check")));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, TEXT("/Game/image/ui/cross")));
}

void UVoteWidget::OnPlayer1ButtonClicked()
{
    static int32 CurrentIndex = 0;
    SetPlayerOccupation(1, OccupationNames[CurrentIndex], OccupationImages[CurrentIndex]);
    CurrentIndex = (CurrentIndex + 1) % OccupationNames.Num();
}

void UVoteWidget::OnPlayer2ButtonClicked()
{
    static int32 CurrentIndex = 0;
    SetPlayerOccupation(2, OccupationNames[CurrentIndex], OccupationImages[CurrentIndex]);
    CurrentIndex = (CurrentIndex + 1) % OccupationNames.Num();
}

void UVoteWidget::OnPlayer3ButtonClicked()
{
    static int32 CurrentIndex = 0;
    SetPlayerOccupation(3, OccupationNames[CurrentIndex], OccupationImages[CurrentIndex]);
    CurrentIndex = (CurrentIndex + 1) % OccupationNames.Num();
}

void UVoteWidget::OnPlayer4ButtonClicked()
{
    static int32 CurrentIndex = 0;
    SetPlayerOccupation(4, OccupationNames[CurrentIndex], OccupationImages[CurrentIndex]);
    CurrentIndex = (CurrentIndex + 1) % OccupationNames.Num();
}

void UVoteWidget::SetPlayerOccupation(int32 PlayerIndex, const FString& OccupationName, UTexture2D* OccupationImage)
{
    UCanvasPanel* TargetCanvas = nullptr;
    UImage* TargetImage = nullptr;
    UTextBlock* TargetText = nullptr;

    switch (PlayerIndex)
    {
        case 1:
            TargetCanvas = Player1CanvasPanel;
            TargetImage = Player1OccupationImage;
            TargetText = Player1NameText;
            break;
        case 2:
            TargetCanvas = Player2CanvasPanel;
            TargetImage = Player2OccupationImage;
            TargetText = Player2NameText;
            break;
        case 3:
            TargetCanvas = Player3CanvasPanel;
            TargetImage = Player3OccupationImage;
            TargetText = Player3NameText;
            break;
        case 4:
            TargetCanvas = Player4CanvasPanel;
            TargetImage = Player4OccupationImage;
            TargetText = Player4NameText;
            break;
    }

    if (TargetCanvas && TargetImage && TargetText)
    {
        TargetCanvas->SetIsEnabled(true);
        TargetCanvas->SetRenderOpacity(1.0f);
        TargetCanvas->SetVisibility(ESlateVisibility::Visible);

        TargetImage->SetBrushFromTexture(OccupationImage);
        TargetText->SetText(FText::FromString(OccupationName));
    }
}