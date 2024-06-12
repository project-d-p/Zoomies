// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DPAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API UDPAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UDPAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* sound;

	UFUNCTION(/*BlueprintCallable*/)
	void AnimNotify_Footstep();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void AnimNotify_foot();
	virtual void AnimNotify_foot_Implementation();
};
