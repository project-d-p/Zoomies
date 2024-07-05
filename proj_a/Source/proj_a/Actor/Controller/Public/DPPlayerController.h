// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ChatManager.h"
#include "ClientSocket.h"
#include "message.pb.h"
#include "SteamNetworkingSocket.h"
#include "DoubleBuffer.h"
#include "DPMySocket.h"
#include "proj_a/Component/InGame/Score/PrivateScoreManager.h"
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

	// void CreateSocket();
	void Connect();
	void RunTask();

	// Server Handling Message
	void HandleMovement(const Movement& movement, const float& server_delta);
	void HandleJump(const Jump& Jump);
	void HandleFire(const Message& fire);
	void HandleCatch(const Message& Msg);
	void HandleAim(const AimState& AimState);
	void HandlePosition(const ActorPosition& ActorPosition);
	
	void SimulateGunFire(SteamNetworkingSocket* steam_socket);
	void SimulateCatch(SteamNetworkingSocket* steam_socket);
	
	UPlayerScoreComp* GetScoreManagerComponent() const;
	UPrivateScoreManager* GetPrivateScoreManagerComponent() const;
	
	void ReleaseMemory();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	class ADPCharacter* character;
	class UDPStateActorComponent* state;
	class UDPConstructionActorComponent* construction;
	
	UPROPERTY()
	UPrivateScoreManager* PrivateScoreManager;

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
	class UInputAction* catchAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* returnAction;

	UPROPERTY()
	UChatManager* ChatManager = nullptr;

	UPROPERTY()
	UClientSocket* Socket = nullptr;

	UPROPERTY()
	class UHitScan* CatchRay = nullptr;

	int gun_fire_count_ = 0;
	std::queue<Message> gun_queue_;
	std::queue<Message> catch_queue_;

	virtual void Tick(float DeltaSeconds) override;
	bool IsCatchable(FHitResult& hit_result);
	
	void Move(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);
	void Active(const FInputActionValue& value);
	void AdditionalSetting(const FInputActionValue& value);
	void Aim(const FInputActionValue& value);
	void AimReleased(const FInputActionValue& value);
	void ActionCancel(const FInputActionValue& value);
	void Catch(const FInputActionValue& value);
	void ReturningAnimals(const FInputActionValue& value);
	
	void SetRotation(const ActorPosition& ActorPosition);
	void SetPosition(const ActorPosition& ActorPosition);
	void SetState(const ActorPosition& ActorPosition);

	// void UpdatePlayer();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* jumpSound;
	std::queue<bool> aim_queue_;
};
