// Fill out your copyright notice in the Description page of Project Settings.

#include "DPCharacter.h"
#include "GameFramework/SpringArmComponent.h"
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

}

// Called to bind functionality to input
void ADPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

