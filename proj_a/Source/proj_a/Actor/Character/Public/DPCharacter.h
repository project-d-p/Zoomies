// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterPositionSync.h"
#include "DynamicTexturedCharacter.h"
#include "NameTag.h"
#include "Components/WidgetComponent.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "DPCharacter.generated.h"

class ABaseMonsterCharacter;
class UTextureTransferManager;

UCLASS()
class PROJ_A_API ADPCharacter : public ADynamicTexturedCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADPCharacter();
	virtual ~ADPCharacter() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// called when the player state changes (e.g. from NULL to a valid player state) both on the server and the client
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Client, Reliable)
	void ClientNotifyAnimalReturn(const FString& player_name);

	TArray<EAnimal> ReturnMonsters();

	UFUNCTION(Server, Reliable)
	void CharacterRun();
	UFUNCTION(Server, Reliable)
	void CharacterRunReleased();


	UPROPERTY(ReplicatedUsing=OnRep_SyncStunned)
	bool bIsStunned;
	UFUNCTION()
	void OnRep_SyncStunned();

	UPROPERTY(ReplicatedUsing=OnRep_SyncRunning)
	bool bIsRunning = false;
	UFUNCTION()
	void OnRep_SyncRunning();
	
	void SetStunned(bool bCond);
	bool IsStunned() const;
	void ApplyStunEffect();
	void RemoveStunEffect();

	UFUNCTION(NetMulticast, Reliable)
	void ApplyKockback(const FHitResult& HitResult);
	UFUNCTION(NetMulticast, Reliable)
	void SetNameTag();
	void UpdateLobbyInfo();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void SetCrownMesh();

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class AReturnTriggerVolume* ReturnTriggerVolume; 

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MatchLobby", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* userInfoWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MatchLobby")
	UWidgetComponent* LobbyInfoWidgetComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MatchLobby")
	UWidgetComponent* LobbyInfoWidgetComponentBack = nullptr;

	UPROPERTY()
	TSubclassOf<UNameTag> NameTag_BP;
	UPROPERTY()
	UNameTag* NameTag_Instance;
	UPROPERTY()
	UWidgetComponent* NameTag_WidgetComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMesh* Crown;
	
	// stun effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* StunEffect;
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* StunArrow;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* StunEffectComponent;
	
protected:
	void OnHostMigration(UWorld* World, UDataManager* DataManager);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ClientNotifyAnimalReturn_Implementation(const FString& player_name);
	void SetCrownMesh_Implementation();
	
private:
	void UpdateNameTagRotation();
	void CheckCollisionWithMonster();
	void OnServerHit(const FHitResult& HitResult);
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArm;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* camera;

	//UPROPERTY(VisibleAnywhere, Category = Camera)
	//class USpringArmComponent* sceneCaptureSpringArm;
	//UPROPERTY(VisibleAnywhere, Category = Camera)
	//class  USceneCaptureComponent2D* sceneCapture;

	UPROPERTY()
	UCharacterPositionSync* syncer = nullptr;

	TSubclassOf<UCameraShakeBase> cameraShake;

	// Collision with monster
	FTimerHandle timerCollisionHandle;
	FDelegateHandle OnHostMigrationDelegate;
	
public:
	FVector currentVelocity{ 0.f, 0.f, 0.f };
	UPROPERTY(BlueprintReadWrite)
	float speed{ 0.f };
	UPROPERTY(BlueprintReadWrite)
	bool isAim{ false };
	bool mIsAtReturnPlace{ true };
	UPROPERTY(BlueprintReadWrite)
	bool isKnockback{ false };

	UPROPERTY(BlueprintReadWrite)
	bool canInteract{ false };
};
