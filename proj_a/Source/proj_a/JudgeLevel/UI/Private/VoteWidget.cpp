// VoteWidget.cpp
#include "VoteWidget.h"

void UVoteWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    InitializeOccupations();
    if (OpenVoteListButton) OpenVoteListButton->OnClicked.AddDynamic(this, &UVoteWidget::OnOpenVoteListButtonClicked);
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

// void UVoteWidget::SetVoterName(const FString& VoterName)
// {
//     
// }

void UVoteWidget::OnOpenVoteListButtonClicked()
{
    VotableListPannel->SetIsEnabled(true);
    VotableListPannel->SetRenderOpacity(1.0f);
    VotableListPannel->SetVisibility(ESlateVisibility::Visible);
}

void UVoteWidget::SetPlayerOccupation(int32 PlayerIndex, const FString& OccupationName, UTexture2D* OccupationImage)
{
    UCanvasPanel* TargetCanvas = nullptr;
    UImage* TargetImage = nullptr;
    UTextBlock* TargetText = nullptr;

    if (TargetCanvas && TargetImage && TargetText)
    {
        TargetCanvas->SetIsEnabled(true);
        TargetCanvas->SetRenderOpacity(1.0f);
        TargetCanvas->SetVisibility(ESlateVisibility::Visible);

        TargetImage->SetBrushFromTexture(OccupationImage);
        TargetText->SetText(FText::FromString(OccupationName));
    }
}