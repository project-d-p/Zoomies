// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ChatManager.h"
#include "message.pb.h"
#include "DoubleBuffer.h"
#include "DPMySocket.h"
#include "DPPlayerController.generated.h"

class UPlayerScoreComp;

UCLASS()
class PROJ_A_API ADPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADPPlayerController();
	
	void SendChatMessageToServer(const FString& Message);
	void ReceiveChatMessage(const FString& SenderName, const FString& Message);
	void InitChatManager(UChatUI* ChatUI);

	void CreateSocket();
	void Connect(FString ip, uint32 port);
	void RunTask();

	void HandleMovement(const Movement& movement);
	
	UPlayerScoreComp* GetScoreManagerComponent() const;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;

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

	UPROPERTY()
	UMySocket *Socket;

	FTimerHandle MovementTimerHandle;
	FTimerHandle SynchronizeHandle;

	FVector2D AccumulatedMovementInput;
	FVector AccumulatedForwardInput;
	FVector AccumulatedRightInput;

	int32 MovementCount = 0;
	int32 ServerReceivedMovementCount = 0;

	void SendCompressedMovement();
	
	// virtual void Tick(float DeltaSeconds) override;
	
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
