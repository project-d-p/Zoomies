// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DPCharacter.generated.h"

UCLASS()
class PROJ_A_API ADPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADPCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// FString playerID;
	// FVector3f pos;
	// FVector3f orientation;
	// FString state;

	// WeaponCompoment* weaponComponent;
	// healthCompoment* healthComponent;
	// StateComponent* stateComponent;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* camera;

public:
	FVector currentVelocity{ 0.f, 0.f, 0.f };
	UPROPERTY(BlueprintReadWrite)
	float speed{ 0.f };
};
