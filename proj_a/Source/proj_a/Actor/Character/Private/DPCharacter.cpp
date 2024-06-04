// Fill out your copyright notice in the Description page of Project Settings.

#include "DPCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "DPHpActorComponent.h"
#include "DPConstructionActorComponent.h"
#include "DPWeaponActorComponent.h"
#include "DPStateActorComponent.h"
#include "DPWeaponGun.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ADPCharacter::ADPCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hpComponent = CreateDefaultSubobject<UDPHpActorComponent>(TEXT("HPComponent"));
	constructionComponent = CreateDefaultSubobject<UDPConstructionActorComponent>(TEXT("ConstructionComponent"));
	weaponComponent = CreateDefaultSubobject<UDPWeaponActorComponent>(TEXT("WeaponComponent"));
	stateComponent = CreateDefaultSubobject<UDPStateActorComponent>(TEXT("StateComponent"));

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

	springArm->TargetArmLength = 700.0f;
	//springArm->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	springArm->bUsePawnControlRotation = true;

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
	sceneCapture->ProjectionType = ECameraProjectionMode::Type::Orthographic;
	sceneCapture->OrthoWidth = 1024.0f;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/steve/characterAnimation.characterAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CHARACTER_MONTAGE
	(TEXT("/Game/animation/steve/characterAnimMontage.characterAnimMontage"));
	if (CHARACTER_MONTAGE.Succeeded()) {
		characterMontage = CHARACTER_MONTAGE.Object;
	}
	// disable move replication : set bReplicateMovement to false
	AActor::SetReplicatingMovement(false);
}

// Called when the game starts or when spawned
void ADPCharacter::BeginPlay()
{
	Super::BeginPlay();

	stateComponent->currentEquipmentState = 0;
	hpComponent->Hp = 100.f;
	hpComponent->IsDead = false;
	constructionComponent->placeWall = false;
	constructionComponent->placeturret = false;
	UE_LOG(LogTemp, Log, TEXT("is it replicaed: %d"), GetCharacterMovement()->GetIsReplicated());
	TSubclassOf<ADPWeapon> gunClass = ADPWeaponGun::StaticClass();
	if (weaponComponent) {
		weaponComponent->AddWeapons(gunClass);
		weaponComponent->Equip(gunClass);
	}
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

	//UE_LOG(LogTemp, Warning, TEXT("speed : %f"), speed);
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
		PlayAnimMontage(characterMontage, 1.f, "aim");	UE_LOG(LogTemp, Warning, TEXT("PlayAimAnimation"));
	}
}

void ADPCharacter::StopAimAnimation()
{
	if (characterMontage) {
		isAim = false;
		StopAnimMontage(characterMontage); UE_LOG(LogTemp, Warning, TEXT("StopAimAnimation"));
	}
}

void ADPCharacter::PlayFireAnimation()
{
	if (characterMontage) {
		PlayAnimMontage(characterMontage, 1.f, "fire");	UE_LOG(LogTemp, Warning, TEXT("PlayFireAnimation"));
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

// disable replication
void ADPCharacter::DisableReplication()
{
	bReplicates = false;
	GetCharacterMovement()->SetIsReplicated(false);
}
