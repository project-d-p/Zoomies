// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.h"
#include "Blueprint/UserWidget.h"
#include "DPJobAssign.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API UDPJobAssign : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnJobAssigned(const EPlayerJob& Job);
};
