// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CHAR_MatchingLobby.generated.h"

UCLASS()
class PROJ_A_API ACHAR_MatchingLobby : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACHAR_MatchingLobby();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:	// component
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MatchLobby")
	class UDPWeaponActorComponent* weaponComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MatchLobby", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* gun;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MatchLobby", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* userInfoWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MatchLobby")
	UAnimMontage* characterMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MatchLobby")
	UWidgetComponent* LobbyInfoWidgetComponent = nullptr;

	void StopAimAnimation();
	void UpdateLobbyInfo();
	
private:

public:
	bool isAim{ false };
};
