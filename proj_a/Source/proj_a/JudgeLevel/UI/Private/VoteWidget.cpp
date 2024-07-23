// VoteWidget.cpp
#include "VoteWidget.h"

#include "OccupationButton.h"
#include "PathManager.h"
#include "TimerUI.h"
#include "Delegates/DelegateSignatureImpl.inl"

void UVoteWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    InitializeOccupations();
    if (OpenVoteListButton) OpenVoteListButton->OnClicked.AddDynamic(this, &UVoteWidget::OnOpenVoteListButtonClicked);
}

void UVoteWidget::InitializeOccupations()
{
    OccupationTypes = {
        EOccupation::ARCHAEOLOGIST,
        EOccupation::POACHER,
        EOccupation::ENVIRONMENTALIST,
        EOccupation::RINGMASTER,
        EOccupation::TERRORIST,
        EOccupation::CHECK,
        EOccupation::CROSS };
    
    checkf(VoteButtonsGrid, TEXT("VoteButtonsGrid is nullptr"))
    for (int32 i = 0; i < OccupationTypes.Num(); ++i)
    {
        UOccupationButton* BT = Cast<UOccupationButton>(VoteButtonsGrid->GetChildAt(i));
        BT->SetOccupation(OccupationTypes[i]);
        BT->SetLambda([this](EOccupation EOcc)
        {
            this->CurrentVoterOcc = EOcc;
            UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(EOcc));
            this->VoterImg->SetBrushFromTexture(Texture);
        });
    }
}

void UVoteWidget::OnOpenVoteListButtonClicked()
{
    VotableListPannel->SetVisibility(
        VotableListPannel->GetVisibility() == ESlateVisibility::Visible
        ? ESlateVisibility::Hidden
        : ESlateVisibility::Visible
    );
}
