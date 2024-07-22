// VoteWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "VoteWidget.generated.h"

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
    
    virtual void NativeConstruct() override;

private:
    TArray<FString> OccupationNames;
    UPROPERTY()
    TArray<UTexture2D*> OccupationImages;

    void SetPlayerOccupation(int32 PlayerIndex, const FString& OccupationName, UTexture2D* OccupationImage);
    void InitializeOccupations();
    UFUNCTION()
    void OnOpenVoteListButtonClicked();
};