// VoteWidget.cpp
#include "VoteWidget.h"

#include "OccupationButton.h"
#include "PathManager.h"
#include "ScoreTypes.h"
#include "TimerUI.h"
#include "Components/SizeBox.h"
#include "Delegates/DelegateSignatureImpl.inl"

void UVoteWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    InitializEPlayerJobs();
    //if (OpenVoteListButton) OpenVoteListButton->OnClicked.AddDynamic(this, &UVoteWidget::OnOpenVoteListButtonClicked);
}

void UVoteWidget::InitializEPlayerJobs()
{
    OccupationTypes = {
        EPlayerJob::JOB_ARCHAEOLOGIST,
        EPlayerJob::JOB_POACHER,
        EPlayerJob::JOB_RINGMASTER,
        EPlayerJob::JOB_ENVIRONMENTALIST,
        EPlayerJob::JOB_TERRORIST,
        };
    
    checkf(VoteButtonsGrid, TEXT("VoteButtonsGrid is nullptr"))
    for (int32 i = 0; i < OccupationTypes.Num(); ++i)
    {
        UWidget* ChildWidget = VoteButtonsGrid->GetChildAt(i);
        if (USizeBox* SizeBox = Cast<USizeBox>(ChildWidget))
        {
            if (UButton* Button = Cast<UButton>(SizeBox->GetChildAt(0)))
            {
                if (UOccupationButton* OccupationButton = Cast<UOccupationButton>(Button))
                {
                    OccupationButton->SetOccupation(OccupationTypes[i]);
                    OccupationButton->SetLambda([this](EPlayerJob EOcc)
                    {
                        this->CurrentVoterOcc = EOcc;
                    });
                }
            }
        }
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
