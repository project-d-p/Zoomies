// Fill out your copyright notice in the Description page of Project Settings.

#include "DPCharacter.h"

#include "BaseMonsterCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DPHpActorComponent.h"
#include "DPConstructionActorComponent.h"
#include "DPPlayerState.h"
#include "DPWeaponActorComponent.h"
#include "DPStateActorComponent.h"
#include "DPWeaponGun.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "MonsterSlotComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ReturnTriggerVolume.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "DSP/LFO.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Serialization/BulkDataRegistry.h"

// Sets default values
ADPCharacter::ADPCharacter()
{
	bReplicates = true;
	
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

	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);

	sceneCaptureSpringArm->SetupAttachment(RootComponent);
	sceneCapture->SetupAttachment(sceneCaptureSpringArm);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CHARACTER
	(TEXT("/Game/model/steve/StickManForMixamo.StickManForMixamo"));
	if (SK_CHARACTER.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_CHARACTER.Object);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 270.f, 0.f));
	GetMesh()->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));

	springArm->TargetArmLength = 400.0f;
	//springArm->SetRelativeLocation(FVector(0.f, 50.f, 150.f));
	//springArm->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
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
	AActor::SetReplicatingMovement(false);
	// bReplicateMovement

	// Set Mass and Collision Profile
	GetCharacterMovement()->Mass = 0.1f;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Enable hit events
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
    
	// Bind the hit event
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ADPCharacter::OnHit);

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
}

// Called when the game starts or when spawned
void ADPCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh()) {
		UMaterialInterface* Material = GetMesh()->GetMaterial(0);
		if (Material) {
			dynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
			GetMesh()->SetMaterial(0, dynamicMaterialInstance);
		}
	}
	if (dynamicMaterialInstance)
		dynamicMaterialInstance->SetVectorParameterValue(FName("color"), FVector4(0.f, 0.f, 1.f, 1.f));

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
	UE_LOG(LogTemp, Log, TEXT("is it replicaed: %d"), GetCharacterMovement()->GetIsReplicated());
	bUseControllerRotationYaw = false;
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
	if (this->GetLocalRole() == ROLE_SimulatedProxy)
	{
		syncer->SyncWithServer(this);
		syncer->SyncGunFire(this);
		syncer->SyncReturnAnimal(this);
	}
	syncer->SyncCatch(this);

	if (bIsStunned)
	{
		// Rotate the arrow component
		FRotator NewRotation = StunArrow->GetRelativeRotation();
		NewRotation.Roll += DeltaTime * 180.0f; // Rotate 180 degrees per second
		StunArrow->SetRelativeRotation(NewRotation);
	}
}

// Called to bind functionality to input
void ADPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool ADPCharacter::IsLocallyControlled() const
{
	// Super::IsLocallyControlled();
	// if (HasAuthority())
	// 	return true;
	return Super::IsLocallyControlled();
}


void ADPCharacter::PlayAimAnimation()
{
	if (characterMontage && !isAim ) {
		isAim = true;
		PlayAnimMontage(characterMontage, 1.f, "aim");	UE_LOG(LogTemp, Warning, TEXT("PlayAimAnimation"));

		springArm->TargetArmLength = 270.0f;
	}
}

void ADPCharacter::StopAimAnimation()
{
	if (characterMontage) {
		isAim = false;
		StopAnimMontage(characterMontage); UE_LOG(LogTemp, Warning, TEXT("StopAimAnimation"));

		springArm->TargetArmLength = 400.0f;
	}
}

void ADPCharacter::PlayFireAnimation()
{
	if (characterMontage) {
		PlayAnimMontage(characterMontage, 1.f, "fire");	UE_LOG(LogTemp, Warning, TEXT("PlayFireAnimation"));
	}

	if (camera && cameraShake) {
		FVector cameraLocation = camera->GetComponentLocation();
		UGameplayStatics::PlayWorldCameraShake(this, cameraShake, cameraLocation, 0.0f, 500.0f);
		FNetLogger::EditerLog(FColor::Magenta, TEXT("CAMERASHAKE"));
	}
}

void ADPCharacter::ChangeAnimation()
{
	if (characterMontage) {
		PlayAnimMontage(characterMontage, 1.f, "changeWeapon");	UE_LOG(LogTemp, Warning, TEXT("ChangeAnimation"));
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

TArray<EAnimal> ADPCharacter::ReturnMonsters()
{
	return monsterSlotComponent->RemoveMonstersFromSlot();
}

void ADPCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (this->IsStunned())
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("It's Already Stunned"));
		return ;
	}
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Collsion Detected"));
	
	ABaseMonsterCharacter* monster = Cast<ABaseMonsterCharacter>(OtherActor);
	if (!monster)
	{
		return ;
	}
	
	if (monster->GetState() == EMonsterState::Faint)
	{
		return ;
	}
	
	this->ApplyStunEffect();
	
	FTimerDelegate timerCollisionDelegate;
	timerCollisionDelegate.BindLambda([this]()
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Stun End"));
		this->RemoveStunEffect();
	});
	float stunTime = 1.0f;
	GetWorld()->GetTimerManager().SetTimer(timerCollisionHandle, timerCollisionDelegate, stunTime, false);
}

void ADPCharacter::ApplyStunEffect()
{
	if (!bIsStunned)
	{
		bIsStunned = true;
		StunEffectComponent->Activate(true);
	}
}

void ADPCharacter::RemoveStunEffect()
{
	bIsStunned = false;
	StunEffectComponent->Deactivate();
}