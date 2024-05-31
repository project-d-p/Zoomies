// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ChatManager.h"
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
	
	void SendChatMessageToServer(const FString& Message);
	void ReceiveChatMessage(const FString& SenderName, const FString& Message);
	void InitChatManager(UChatUI* ChatUI);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	class ADPCharacter* character;
	class UDPStateActorComponent* state;
	class UDPConstructionActorComponent* construction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* defaultContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* moveAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* jumpAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* rotateAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* activeAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* additionalSettingAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* aimAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* cancelAction;

	UPROPERTY()
	UChatManager* ChatManager = nullptr;
	
	virtual void Tick(float DeltaSeconds) override;
	
	void Move(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);
	void Active(const FInputActionValue& value);
	void AdditionalSetting(const FInputActionValue& value);
	void Aim(const FInputActionValue& value);
	void AimReleased(const FInputActionValue& value);
	void ActionCancel(const FInputActionValue& value);
	
	void UpdatePlayer(/*DataHub result*/);
};
