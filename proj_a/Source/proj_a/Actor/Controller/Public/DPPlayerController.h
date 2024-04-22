// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "DPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADPPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	class ADPCharacter* character;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* defaultContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* moveAction;
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* jumpAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* rotateAction;

	void Move(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);

	void UpdatePlayer(/*DataHub result*/);
};
