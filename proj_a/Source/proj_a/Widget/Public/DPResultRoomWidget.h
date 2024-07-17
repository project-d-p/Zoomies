// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DPResultRoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API UDPResultRoomWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

public:
    //UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    //class UVerticalBox* scoresVerticalBox;

    UFUNCTION(BlueprintCallable, Category = "UI")
    TArray<int32> UpdateScores(/*const TArray<FString>& names, */const TArray<int32>& scores);

    //UFont* loadFont;
};
