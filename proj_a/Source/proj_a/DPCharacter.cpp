// Fill out your copyright notice in the Description page of Project Settings.

#include "DPCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ADPCharacter::ADPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	
	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CHARACTER
	(TEXT("/Game/model/player/Swat.Swat"));
	if (SK_CHARACTER.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_CHARACTER.Object);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 270.f, 0.f));

	springArm->TargetArmLength = 700.0f;
	//springArm->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	springArm->bUsePawnControlRotation = true;

	// 움직이는 방향을 자동으로 보게
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// 애니메이션 모드, 클래스 적용
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/blueprints/characterAnimation.characterAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}
}

// Called when the game starts or when spawned
void ADPCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 현재 속도 벡터 구하고 크기
	if (GetCharacterMovement()) {
		currentVelocity = GetCharacterMovement()->Velocity;
		speed = currentVelocity.Size();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("mmmmmmmmmmmmmmmmmmmmmmmm"));

	UE_LOG(LogTemp, Warning, TEXT("speed : %f"), speed);
}

// Called to bind functionality to input
void ADPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

