// Fill out your copyright notice in the Description page of Project Settings.

#include "DPCharacter.h"

#include "BaseMonsterCharacter.h"
#include "ChasePlayerMonsterAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DPHpActorComponent.h"
#include "DPConstructionActorComponent.h"
#include "DPPlayerState.h"
#include "DPWeaponActorComponent.h"
#include "DPStateActorComponent.h"
#include "DPWeaponGun.h"
#include "DynamicTextureComponent.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "MonsterSlotComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "proj_a.h"
#include "ReturnTriggerVolume.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "DSP/LFO.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/BulkDataRegistry.h"

// Sets default values
ADPCharacter::ADPCharacter()
{
	bReplicates = true;
	bIsStunned = false;
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hpComponent = CreateDefaultSubobject<UDPHpActorComponent>(TEXT("HPComponent"));
	constructionComponent = CreateDefaultSubobject<UDPConstructionActorComponent>(TEXT("ConstructionComponent"));
	weaponComponent = CreateDefaultSubobject<UDPWeaponActorComponent>(TEXT("WeaponComponent"));
	stateComponent = CreateDefaultSubobject<UDPStateActorComponent>(TEXT("StateComponent"));
	monsterSlotComponent = CreateDefaultSubobject<UMonsterSlotComponent>(TEXT("MonsterSlotComponent"));

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	sceneCaptureSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SCENECAPTURESPRINGARM"));
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SCENECAPTURE"));

	// tag
	Tags.Add(FName("player"));

	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);

	sceneCaptureSpringArm->SetupAttachment(RootComponent);
	sceneCapture->SetupAttachment(sceneCaptureSpringArm);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CHARACTER
	(TEXT("/Game/model/steve/StickManForMixamo.StickManForMixamo"));
	if (SK_CHARACTER.Succeeded()) 
	{
		DynamicTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("DynamicTextureComponent"));
		DynamicTextureComponent->InitializeTexture();
		const FString FilePath = FPaths::ProjectContentDir() + TEXT("customCharacter/test4.png");
		DynamicTextureComponent->LoadTextureFromFile(FilePath);
		
		GetMesh()->SetSkeletalMesh(SK_CHARACTER.Object);

		dynamicMaterialInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this, TEXT("DynamicMaterial"));
		
		if (dynamicMaterialInstance)
		{
			dynamicMaterialInstance->SetTextureParameterValue(TEXT("renderTarget"), DynamicTextureComponent->GetDynamicTexture());
			GetMesh()->SetMaterial(0, dynamicMaterialInstance);
		}
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 270.f, 0.f));
	GetMesh()->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));

	springArm->TargetArmLength = 400.0f;
	springArm->bUsePawnControlRotation = true;
	camera->SetRelativeLocation(FVector(0.f, 50.f, 150.f));

	// minimap
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RENDERTARGET
	(TEXT("/Game/widget/minimap/rt_minimap.rt_minimap"));
	if (RENDERTARGET.Succeeded()){
		sceneCapture->TextureTarget = RENDERTARGET.Object;
	}

	sceneCaptureSpringArm->TargetArmLength = 700.0f;
	sceneCaptureSpringArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	sceneCaptureSpringArm->bInheritPitch = false;
	sceneCaptureSpringArm->bInheritYaw = false;
	sceneCaptureSpringArm->bInheritRoll = false;
	sceneCaptureSpringArm->bDoCollisionTest = false;
	sceneCapture->ProjectionType = ECameraProjectionMode::Type::Orthographic;
	sceneCapture->OrthoWidth = 1024.0f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/steve/steveAnimation.steveAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CHARACTER_MONTAGE
	(TEXT("/Game/animation/steve/characterAnimMontage.characterAnimMontage"));
	if (CHARACTER_MONTAGE.Succeeded()) {
		characterMontage = CHARACTER_MONTAGE.Object;
	}

	syncer = CreateDefaultSubobject<UCharacterPositionSync>(TEXT("My Syncer"));


	// disable move replication : set bReplicateMovement to false
	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = World->GetMapName();
		if (CurrentLevelName.Contains("mainLevel"))
		{
			AActor::SetReplicatingMovement(false);
		}
		else
		{
			AActor::SetReplicatingMovement(true);
		}
	}
	// Set Mass and Collision Profile
	GetCharacterMovement()->Mass = 0.1f;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerChannel);
	GetMesh()->SetCollisionObjectType(ECC_PlayerChannel);

	// Enable hit events
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->CustomDepthStencilValue = 2;

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CAMERASHAKE
	(TEXT("/Game/etc/bp_cameraShake.bp_cameraShake_C"));
	if (CAMERASHAKE.Succeeded()) {
		cameraShake = CAMERASHAKE.Class;
	}
	
	StunArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StunArrow"));
	StunArrow->SetupAttachment(GetMesh(), FName("Stun_Pos"));
	StunArrow->SetRelativeLocation(FVector(0, -20.f, 0));
	StunArrow->SetHiddenInGame(true);
	
	StunEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StunEffectComponent"));
	StunEffectComponent->SetupAttachment(StunArrow);
	StunEffectComponent->SetAutoActivate(false);
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> STUN
	(TEXT("/Game/effect/ns_stun.ns_stun"));
	if (STUN.Succeeded()) {
		StunEffect = STUN.Object;
		StunEffectComponent->SetAsset(StunEffect);
	}

	static ConstructorHelpers::FClassFinder<UNameTag> NAME_TAG
	(TEXT("/Game/widget/widget_NameTag.widget_NameTag_C"));
	if (NAME_TAG.Succeeded())
	{
		NameTag_BP = NAME_TAG.Class;
		
		NameTag_WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTag_WidgetComponent"));
		NameTag_WidgetComponent->SetupAttachment(RootComponent);
		NameTag_WidgetComponent->SetDrawAtDesiredSize(true);
		NameTag_WidgetComponent->SetRelativeLocation(FVector(0, 0, 100));
		NameTag_WidgetComponent->SetWorldScale3D(FVector(0.6f, 0.6f, 0.6f));
	}

	postProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("postProcessComponent"));
	postProcessComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> PostProcessMaterial
	(TEXT("MaterialInstanceConstant'/Game/material/postprocess/mpp_dizzle_Inst.mpp_dizzle_Inst'"));
	if (PostProcessMaterial.Succeeded())
	{
		postProcessComponent->AddOrUpdateBlendable(PostProcessMaterial.Object, 1.0f);
		postProcessComponent->BlendWeight = 0.0f;
	}

	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
		
		if (CurrentLevelName == "matchLobby")
		{
			if (LobbyInfoWidgetComponent && LobbyInfoWidgetComponent->IsValidLowLevel())
			{
				LobbyInfoWidgetComponent->DestroyComponent();
				LobbyInfoWidgetComponent = nullptr;
			}
	
			LobbyInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LobbyInfoWidgetComponent"));
			static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/widget/widget_LobbyInfo.widget_LobbyInfo_C"));
			if (WidgetClass.Succeeded())
			{
				LobbyInfoWidgetComponent->SetWidgetClass(WidgetClass.Class);
			}
			LobbyInfoWidgetComponent->SetVisibility(true);
			LobbyInfoWidgetComponent->SetWidgetSpace( EWidgetSpace::World);
			LobbyInfoWidgetComponent->SetupAttachment(GetMesh());
			LobbyInfoWidgetComponent->SetRelativeLocation(FVector(0, 0, 650));
			LobbyInfoWidgetComponent->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));
			LobbyInfoWidgetComponent->SetDrawSize(FVector2D(260,100));
			LobbyInfoWidgetComponent->SetRelativeRotation(FRotator(-180, -90, 180));
		}
	}
}

ADPCharacter::~ADPCharacter()
{
}

void ADPCharacter::SetNameTag()
{
	APlayerState* PS = GetPlayerState();
	if (IsValid(PS))
	{
		FString Name = PS->GetPlayerName();
		if (IsValid(NameTag_Instance))
		{
			NameTag_Instance->SetName(Name);
		}
		else
		{
			FNetLogger::LogError(TEXT("No NameTag_Instance"));
		}
		if (IsValid(NameTag_WidgetComponent) && !IsLocallyControlled())
		{
			NameTag_WidgetComponent->SetVisibility(true);
		}
		else
		{
			FNetLogger::LogError(TEXT("No NameTagWidgetComponent || IsLocallyControlled"));
		}
	}
	else
	{
		static int32 RetryCount = 0;
		if (RetryCount < 5) // �ִ� 5ȸ�� �õ�
		{
			RetryCount++;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				SetNameTag();
			}), 0.1f, false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to set NameTag after multiple attempts."));
		}
	}
}


// Called when the game starts or when spawned
void ADPCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!NameTag_BP)
	{
		check(false);
	}
	NameTag_Instance = CreateWidget<UNameTag>(GetWorld(), NameTag_BP);
	if (NameTag_Instance)
	{
		NameTag_WidgetComponent->SetWidget(NameTag_Instance);
		NameTag_WidgetComponent->SetVisibility(false);
	}
	
	//if (GetMesh()) {
	//	UMaterialInterface* Material = GetMesh()->GetMaterial(0);
	//	if (Material) {
	//		dynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	//		GetMesh()->SetMaterial(0, dynamicMaterialInstance);
	//	}
	//}
	//if (dynamicMaterialInstance)
	//	dynamicMaterialInstance->SetVectorParameterValue(FName("color"), FVector4(0.f, 0.f, 1.f, 1.f));

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReturnTriggerVolume::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		ReturnTriggerVolume = Cast<AReturnTriggerVolume>(FoundActors[0]);
	}

	stateComponent->currentEquipmentState = 0;
	hpComponent->Hp = 100.f;
	hpComponent->IsDead = false;
	constructionComponent->placeWall = false;
	constructionComponent->placeturret = false;
	bUseControllerRotationYaw = false;
	SetNameTag();
}

// Called every frame
void ADPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()) {
		currentVelocity = GetCharacterMovement()->Velocity;
		speed = currentVelocity.Size();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("null GetCharacterMovement"));
	
	if (!IsLocallyControlled())
	{
		UpdateNameTagRotation();
	}

	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = World->GetMapName();
		if (CurrentLevelName.Contains("mainLevel") == false)
		{
			return ;
		}
	}
	
	if (this->GetLocalRole() == ROLE_SimulatedProxy)
	{
		syncer->SyncWithServer(this);
		syncer->SyncGunFire(this);
		syncer->SyncReturnAnimal(this);
	}
	if (!HasAuthority())
	{
		syncer->SyncCatch(this);
	}

	if (bIsStunned)
	{
		// Rotate the arrow component
		FRotator NewRotation = StunArrow->GetRelativeRotation();
		NewRotation.Roll += DeltaTime * 180.0f; // Rotate 180 degrees per second
		StunArrow->SetRelativeRotation(NewRotation);
	}

	if (HasAuthority())
	{
		CheckCollisionWithMonster();
	}
}

// Called to bind functionality to input
void ADPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADPCharacter::PlayAimAnimation()
{
	if (characterMontage && !isAim ) {
		isAim = true;
		PlayAnimMontage(characterMontage, 1.f, "aim");

		springArm->TargetArmLength = 270.0f;
	}
}

void ADPCharacter::StopAimAnimation()
{
	if (characterMontage) {
		isAim = false;
		StopAnimMontage(characterMontage);

		springArm->TargetArmLength = 400.0f;
	}
}

void ADPCharacter::PlayFireAnimation()
{
	if (characterMontage) {
		PlayAnimMontage(characterMontage, 1.f, "fire");
	}

	if (camera && cameraShake) {
		FVector cameraLocation = camera->GetComponentLocation();
		UGameplayStatics::PlayWorldCameraShake(this, cameraShake, cameraLocation, 0.0f, 500.0f);
	}
}

void ADPCharacter::ChangeAnimation()
{
	if (characterMontage) {
		PlayAnimMontage(characterMontage, 1.f, "changeWeapon");
	}
}

void ADPCharacter::PlaceConstructionAnimation()
{
}

void ADPCharacter::DestroyConstructionAnimation()
{
}

void ADPCharacter::DyingAnimation()
{
}

bool ADPCharacter::CatchMonster(const FString& monster_type)
{
	return monsterSlotComponent->AddMonsterToSlot(this, monster_type);
}

void ADPCharacter::SetAtReturnPlace(bool isReturnPlace)
{
	this->mIsAtReturnPlace = isReturnPlace;
}

bool ADPCharacter::IsAtReturnPlace() const
{
	return this->mIsAtReturnPlace;
}

void ADPCharacter::RemoveSpringArm()
{
	camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void ADPCharacter::SetStunned(bool bCond)
{
	this->bIsStunned = bCond;
}

bool ADPCharacter::IsStunned() const
{
	return this->bIsStunned;
}

void ADPCharacter::ClientNotifyAnimalReturn_Implementation(const FString& player_name)
{
	FDataHub::PushReturnAnimalDA(player_name, true);
}

void ADPCharacter::UpdateNameTagRotation()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return ;
	}
	if (NameTag_WidgetComponent)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			// FNetLogger::LogError(TEXT("Here is Wrong! : PlayerController"));
			if (!PlayerController->PlayerCameraManager)
			{
				return ;
			}
			FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
			FRotator NewRotation = (CameraLocation - NameTag_WidgetComponent->GetComponentLocation()).Rotation();
           
			NewRotation.Pitch = 0.0f;
			NewRotation.Roll = 0.0f;
            
			NameTag_WidgetComponent->SetWorldRotation(NewRotation);
		}
	}
}

void ADPCharacter::CheckCollisionWithMonster()
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FVector> Direction = {
		FVector::ForwardVector,
		-FVector::UpVector,
	};

	for (int i = 0; i < Direction.Num(); i++)
	{
		if (GetWorld()->SweepSingleByChannel(
			HitResult,
			GetActorLocation(),
			GetActorLocation() + Direction[i] * 50.f,
			GetActorRotation().Quaternion(),
			ECC_MonsterChannel,
			GetCapsuleComponent()->GetCollisionShape(),
			QueryParams
		))
		{
			if (ABaseMonsterCharacter* MC = Cast<ABaseMonsterCharacter>(HitResult.GetActor()))
			{
				/* Only ChasePlayerMonsterAIController can hit the player */
				AChasePlayerMonsterAIController* HC = Cast<AChasePlayerMonsterAIController>(MC->GetController());
				if (!HC)
					continue;
				
				ACharacter* TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), HC->GetPlayerIndex());
				if (TargetPlayer && TargetPlayer == this)
				{
					/* Monster collided with its current target player; initiate search for a new player target */
					HC->SetRandomPlayerIndex();
				}
				OnServerHit(HitResult);
			}
		}
	}
}

TArray<EAnimal> ADPCharacter::ReturnMonsters()
{
	return monsterSlotComponent->RemoveMonstersFromSlot();
}

void ADPCharacter::ApplyStunEffect()
{
	if (!bIsStunned)
	{
		bIsStunned = true;
		StunEffectComponent->Activate(true);
	}
	if (!HasAuthority())
	{
		StunEffectComponent->Activate(true);
	}
	if (IsLocallyControlled()) {
		postProcessComponent->BlendWeight = 1.f;
	}
}

void ADPCharacter::RemoveStunEffect()
{
	if (!IsValid(this)) return;
	if (HasAuthority())
	{
		bIsStunned = false;
		bIsInvincible = true;
		FTimerHandle TimerInvincibleHandle;
		FTimerDelegate TimerInvincibleDelegate;
		TWeakObjectPtr<ADPCharacter> WeakThis(this);
		TimerInvincibleDelegate.BindLambda([WeakThis, this]()
		{
			if (WeakThis.IsValid())
			{
				bIsInvincible = false;
			}
		});
		GetWorld()->GetTimerManager().SetTimer(TimerInvincibleHandle, TimerInvincibleDelegate, 2.0f, false);
	}
	StunEffectComponent->Deactivate();
	// TODO: Invincible Effect
	if (IsLocallyControlled()) {
		postProcessComponent->BlendWeight = 0.f;
	}
}

void ADPCharacter::ApplyKockback_Implementation(const FHitResult& HitResult)
{
	// �浹 �������� ĳ���� ��ġ���� ������ ���
	FVector KnockbackDirection = GetActorLocation() - HitResult.ImpactPoint;
	// FVector KnockbackDirection = -HitResult.ImpactNormal;
	KnockbackDirection.Z = 20.0f;
    
	if (!KnockbackDirection.IsNearlyZero())
	{
		KnockbackDirection.Normalize();
	}
	else
	{
		KnockbackDirection = GetActorForwardVector() * -1;
	}

	// �˹� �ӵ� ����
	float KnockbackSpeed = 2000.0f;

	// Character Movement Component ��������
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		// Save previous movement mode
		EMovementMode PreviousMovementMode = MovementComponent->MovementMode;

		// Set Falling Movement Mode
		MovementComponent->SetMovementMode(MOVE_Falling);

		// Set Knockback Velocity
		MovementComponent->Velocity = KnockbackDirection * KnockbackSpeed;

		FTimerHandle ResetTimerHandle;
		TWeakObjectPtr<ADPCharacter> WeakThis(this);
		GetWorldTimerManager().SetTimer(ResetTimerHandle, [WeakThis, this, MovementComponent, PreviousMovementMode]() {
			if (WeakThis.IsValid())
			{
				// Reset Movement Mode
				MovementComponent->SetMovementMode(PreviousMovementMode);
				isKnockback = false;
			}
		}, 0.5f, false);
	}
}

void ADPCharacter::OnRep_SyncInvincible()
{
}

bool ADPCharacter::IsInvincible()
{
	return bIsInvincible;
}

void ADPCharacter::OnServerHit(const FHitResult& HitResult)
{
	if (this->IsStunned())
	{
		return ;
	}

	if (this->IsInvincible())
	{
		return ;
	}
	
	ABaseMonsterCharacter* monster = Cast<ABaseMonsterCharacter>(HitResult.GetActor());
	if (!monster)
	{
		return ;
	}
	
	if (monster->GetState() == EMonsterState::Faint)
	{
		return ;
	}
	this->ApplyKockback(HitResult);
	this->ApplyStunEffect();
	
	FTimerDelegate timerCollisionDelegate;

	TWeakObjectPtr<ADPCharacter> WeakThis(this);
	timerCollisionDelegate.BindLambda([WeakThis, this]()
	{
		if (WeakThis.IsValid())
		{
			this->RemoveStunEffect();
		}
	});
	float stunTime = 1.5f;
	GetWorld()->GetTimerManager().SetTimer(timerCollisionHandle, timerCollisionDelegate, stunTime, false);
}

void ADPCharacter::OnRep_SyncStunned()
{
	if (bIsStunned)
	{
		this->ApplyStunEffect();
	}
	else
	{
		this->RemoveStunEffect();
	}
}

FVector ADPCharacter::GetCameraLocation() const
{
	FVector cameraLocation = camera->GetComponentLocation();
	return cameraLocation;
}

void ADPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPCharacter, bIsStunned);
}
