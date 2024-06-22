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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDPWeaponActorComponent* weaponComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* gun;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* userInfoWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* characterMontage;

	void StopAimAnimation();
	
private:

public:
	bool isAim{ false };
};
