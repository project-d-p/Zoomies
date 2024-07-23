// VoteWidget.cpp
#include "VoteWidget.h"

#include "PathManager.h"

void UVoteWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    InitializeOccupations();
    InitializeButtons();
    if (OpenVoteListButton) OpenVoteListButton->OnClicked.AddDynamic(this, &UVoteWidget::OnOpenVoteListButtonClicked);
}

void UVoteWidget::InitializeOccupations()
{
    OccupationTypes = { ARCHAEOLOGIST, POACHER, ENVIRONMENTALIST, RINGMASTER, TERRORIST, CHECK, CROSS };
    
    // Load occupation images
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[0])));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[1])));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[2])));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[3])));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[4])));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[5])));
    OccupationImages.Add(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[6])));
}

// void UVoteWidget::SetVoterName(const FString& VoterName)
// {
//     
// }

void UVoteWidget::OnOpenVoteListButtonClicked()
{
    VotableListPannel->SetVisibility(
        VotableListPannel->GetVisibility() == ESlateVisibility::Visible
        ? ESlateVisibility::Hidden
        : ESlateVisibility::Visible
    );
}

void UVoteWidget::InitializeButtons()
{
    for (EOccupation& type : OccupationTypes)
    {
        OBA.Add(FOccupationButton(type));
    }

    checkf(VoteButtonsGrid, TEXT("VoteButtonsGrid is nullptr"))
    for (int32 i = 0; i < VoteButtonsGrid->GetChildrenCount(); ++i)
    {
        UButton* Button = Cast<UButton>(VoteButtonsGrid->GetChildAt(i));

        checkf(Button && i < OBA.Num(), TEXT("Invalid Button or OBA index"))
        OBA[i].Button = Button;
    }
}
