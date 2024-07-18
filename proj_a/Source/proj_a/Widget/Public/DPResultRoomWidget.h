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
public:
	// virtual void NativeConstruct() override;
	
    UFUNCTION(BlueprintCallable, Category = "UI")
    TArray<int32> UpdateScores(const TArray<int32>& scores);
};
