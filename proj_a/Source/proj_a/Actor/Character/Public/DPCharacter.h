// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterPositionSync.h"
#include "NameTag.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "DPCharacter.generated.h"

class ABaseMonsterCharacter;

UCLASS()
class PROJ_A_API ADPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADPCharacter();
	virtual ~ADPCharacter() override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Client, Reliable)
	void ClientNotifyAnimalReturn(const FString& player_name);

	TArray<EAnimal> ReturnMonsters();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class AReturnTriggerVolume* ReturnTriggerVolume; 

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcess")
	class UPostProcessComponent* postProcessComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* characterMontage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	//UMaterialInstanceDynamic* dynamicMaterialInstance;

	UPROPERTY()
	TSubclassOf<UNameTag> NameTag_BP;
	UPROPERTY()
	UNameTag* NameTag_Instance;
	UPROPERTY()
	UWidgetComponent* NameTag_WidgetComponent;
	
	// stun effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* StunEffect;
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* StunArrow;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* StunEffectComponent;

	UPROPERTY(ReplicatedUsing=OnRep_SyncStunned)
	bool bIsStunned;
	UFUNCTION()
	void OnRep_SyncStunned();
	void SetStunned(bool bCond);
	bool IsStunned() const;
	void ApplyStunEffect();
	void RemoveStunEffect();

	UFUNCTION(NetMulticast, Reliable)
	void ApplyKockback(const FHitResult& HitResult);

	UPROPERTY(ReplicatedUsing=OnRep_SyncInvincible)
	bool bIsInvincible = false;
	UFUNCTION()
	void OnRep_SyncInvincible();
	bool IsInvincible();

	FVector GetCameraLocation() const;

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

	void RemoveSpringArm();
	
protected:
	void ClientNotifyAnimalReturn_Implementation(const FString& player_name);
	
private:
	void SetNameTag();
	void UpdateNameTagRotation();
	void CheckCollisionWithMonster();
	void OnServerHit(const FHitResult& HitResult);

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

	TSubclassOf<UCameraShakeBase> cameraShake;

	// Collision with monster
	FTimerHandle timerCollisionHandle;
	
public:
	FVector currentVelocity{ 0.f, 0.f, 0.f };
	UPROPERTY(BlueprintReadWrite)
	float speed{ 0.f };
	bool isAim{ false };
	bool mIsAtReturnPlace{ true };
	UPROPERTY(BlueprintReadWrite)
	bool isKnockback{ false };
};
