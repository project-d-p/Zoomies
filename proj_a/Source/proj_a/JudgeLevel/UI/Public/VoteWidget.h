// VoteWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "ScoreTypes.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "VoteWidget.generated.h"

// refer ScoreTypes.h
// enum EPlayerJob : uint8

USTRUCT()
struct FOccupationData
{
    GENERATED_BODY()

    UPROPERTY()
    EPlayerJob Occupation;
    UPROPERTY()
    UButton* Button;

    FOccupationData() : Occupation(), Button(nullptr) {}
    FOccupationData(EPlayerJob Occ) : Occupation(Occ), Button(nullptr) {}
};

UCLASS()
class PROJ_A_API UVoteWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    EPlayerJob GetVote() const { return CurrentVoterOcc; }
    void SetVoterName(const FString& Name)
    {
        VoterName->SetText(FText::FromString(Name));
        TargetUserName->SetText(FText::FromString(Name));
    }

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* VoteButtonsGrid;
    TArray<EPlayerJob> OccupationTypes;
    EPlayerJob CurrentVoterOcc;
    
protected:
    UPROPERTY(meta = (BindWidget))
    UButton* OpenVoteListButton;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* VoterName;
    UPROPERTY(meta = (BindWidget))
    UImage* VoterImg;
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* VotableListPannel;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TargetUserName;
    
    
    virtual void NativeConstruct() override;

private:

    void InitializEPlayerJobs();
    UFUNCTION()
    void OnOpenVoteListButtonClicked();
};