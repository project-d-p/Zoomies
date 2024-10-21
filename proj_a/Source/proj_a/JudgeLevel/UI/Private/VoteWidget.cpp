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
        UUserWidget* BT = Cast<UUserWidget>(VoteButtonsGrid->GetChildAt(i));
        if (BT)
        {
            UButton* Button = Cast<UButton>(BT->GetWidgetFromName(TEXT("introduce_Button")));
            if (Button)
            {
                UOccupationButton* OccupationButton = Cast<UOccupationButton>(Button);
                if (OccupationButton)
                {
                    OccupationButton->SetOccupation(OccupationTypes[i]);
                    OccupationButton->SetLambda([this](EPlayerJob EOcc)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UJudgeInputComponent::Esc"));
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
