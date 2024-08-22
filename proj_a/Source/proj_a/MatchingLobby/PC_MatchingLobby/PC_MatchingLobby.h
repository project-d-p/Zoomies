#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API APC_MatchingLobby : public APlayerController
{
	GENERATED_BODY()

public:
	APC_MatchingLobby();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 이동 관련 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMoveRight(float Value);

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	// 준비 상태 관련 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetReady(bool pIsReady);

	UFUNCTION(BlueprintCallable, Category = "MatchingLobby")
	void ToggleReadyState();

	void SetCineCameraView();

	UPROPERTY(BlueprintReadWrite, Category = "MatchingLobbyInfo")
	bool bIsReady = false;

	UPROPERTY(BlueprintReadWrite, Category = "MatchingLobbyInfo")
	FString UserName = "DefaultName";

private:
	APawn* ControlledPawn;
};
