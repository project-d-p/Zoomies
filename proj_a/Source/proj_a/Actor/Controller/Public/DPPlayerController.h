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
	class UDPStateActorComponent* state;
	class UDPConstructionActorComponent* construction;

private:
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
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* chatAction;

	virtual void Tick(float DeltaSeconds) override;
	
	void Move(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);
	void Active(const FInputActionValue& value);
	void AdditionalSetting(const FInputActionValue& value);
	void Aim(const FInputActionValue& value);
	void AimReleased(const FInputActionValue& value);
	void ActionCancel(const FInputActionValue& value);
	void OpenChat(const FInputActionValue& value);

	// 클라이언트에서 서버로 채팅 메시지 전송 함수 선언
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FString& Message);

	// 서버에서 클라이언트로 채팅 메시지 수신 함수 선언
	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FString& SenderName, const FString& Message);

	void UpdatePlayer(/*DataHub result*/);
};
