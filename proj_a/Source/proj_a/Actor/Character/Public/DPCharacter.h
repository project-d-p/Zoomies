// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterPositionSync.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DPCharacter.generated.h"

class ABaseMonsterCharacter;

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

	// Locally Controlled
	virtual bool IsLocallyControlled() const override;
	void ReturnMonsters();

public:	// component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDPHpActorComponent* hpComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDPConstructionActorComponent* constructionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDPWeaponActorComponent* weaponComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDPStateActorComponent* stateComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UMonsterSlotComponent* monsterSlotComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* characterMontage;

	void PlayAimAnimation();
	void StopAimAnimation();
	void PlayFireAnimation();
	void ChangeAnimation();
	void PlaceConstructionAnimation();
	void DestroyConstructionAnimation();
	void DyingAnimation();

	bool CatchMonster(const FString& monster_type);

	void SetAtReturnPlace(bool isReturnPlace);
	bool IsAtReturnPlace() const;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* camera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* sceneCaptureSpringArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class  USceneCaptureComponent2D* sceneCapture;

	UPROPERTY()
	UCharacterPositionSync* syncer = nullptr;

	bool isAtReturnPlace{ false };
	
public:
	FVector currentVelocity{ 0.f, 0.f, 0.f };
	UPROPERTY(BlueprintReadWrite)
	float speed{ 0.f };
	bool isAim{ false };
};
