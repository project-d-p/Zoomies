// VoteWidget.cpp
#include "VoteWidget.h"

#include "OccupationButton.h"
#include "PathManager.h"
#include "ScoreTypes.h"
#include "TimerUI.h"
#include "Delegates/DelegateSignatureImpl.inl"

void UVoteWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    InitializEPlayerJobs();
    if (OpenVoteListButton) OpenVoteListButton->OnClicked.AddDynamic(this, &UVoteWidget::OnOpenVoteListButtonClicked);
}

void UVoteWidget::InitializEPlayerJobs()
{
    OccupationTypes = {
        EPlayerJob::JOB_ARCHAEOLOGIST,
        EPlayerJob::JOB_POACHER,
        EPlayerJob::JOB_RINGMASTER,
        EPlayerJob::JOB_ENVIRONMENTALIST,
        EPlayerJob::JOB_TERRORIST,
        EPlayerJob::JOB_CHECK,
        EPlayerJob::JOB_CROSS };
    
    checkf(VoteButtonsGrid, TEXT("VoteButtonsGrid is nullptr"))
    for (int32 i = 0; i < OccupationTypes.Num(); ++i)
    {
        UOccupationButton* BT = Cast<UOccupationButton>(VoteButtonsGrid->GetChildAt(i));
        UImage* BtImg = Cast<UImage>(BT->GetChildAt(0));
        check(BtImg)
        BtImg->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, PathManager::GetOccupationImagePath(OccupationTypes[i])));
        BT->SetOccupation(OccupationTypes[i]);
        BT->SetLambda([this](EPlayerJob EOcc)
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
