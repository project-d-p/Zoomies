// VoteWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "VoteWidget.generated.h"

// refer PathManager.h
// enum EOccupation : uint8

USTRUCT()
struct FOccupationButton
{
    GENERATED_BODY()

    EOccupation Occupation;
    UPROPERTY()
    UButton* Button;

    FOccupationButton() = delete;
    FOccupationButton(EOccupation Occ) : Occupation(Occ), Button(nullptr) {}
};

UCLASS()
class PROJ_A_API UVoteWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    // void SetVoterName(const FString& VoterName);
    
protected:
    UPROPERTY(meta = (BindWidget))
    UButton* OpenVoteListButton;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* VoterName;
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* VotableListPannel;
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* VoteButtonsGrid;
    
    virtual void NativeConstruct() override;

private:
    TArray<EOccupation> OccupationTypes;
    UPROPERTY()
    TArray<UTexture2D*> OccupationImages;
    TArray<FOccupationButton> OBA;
    
    void InitializeButtons();
    void InitializeOccupations();
    UFUNCTION()
    void OnOpenVoteListButtonClicked();
};