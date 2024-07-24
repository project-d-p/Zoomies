// VoteWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "EnumTypes.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "VoteWidget.generated.h"

// refer PathManager.h
// enum EOccupation : uint8

USTRUCT()
struct FOccupationData
{
    GENERATED_BODY()

    UPROPERTY()
    EOccupation Occupation;
    UPROPERTY()
    UButton* Button;

    FOccupationData() : Occupation(), Button(nullptr) {}
    FOccupationData(EOccupation Occ) : Occupation(Occ), Button(nullptr) {}
};

UCLASS()
class PROJ_A_API UVoteWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    EOccupation GetVote() const { return CurrentVoterOcc; }
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
    UUniformGridPanel* VoteButtonsGrid;
    
    virtual void NativeConstruct() override;

private:
    TArray<EOccupation> OccupationTypes;
    EOccupation CurrentVoterOcc;

    void InitializeOccupations();
    UFUNCTION()
    void OnOpenVoteListButtonClicked();
};