// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"
#include "DPCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "DPWeaponActorComponent.h"
#include "DPStateActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADPPlayerController::ADPPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		defaultContext = DEFAULT_CONTEXT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_MOVE
	(TEXT("/Game/input/ia_move.ia_move"));
	if (IA_MOVE.Succeeded())
		moveAction = IA_MOVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_JUMP
	(TEXT("/Game/input/ia_jump.ia_jump"));
	if (IA_JUMP.Succeeded())
		jumpAction = IA_JUMP.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ROTATE
	(TEXT("/Game/input/ia_rotate.ia_rotate"));
	if (IA_ROTATE.Succeeded())
		rotateAction = IA_ROTATE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ACTIVE
	(TEXT("/Game/input/ia_active.ia_active"));
	if (IA_ACTIVE.Succeeded())
		activeAction = IA_ACTIVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ADDITIONALSETTING
	(TEXT("/Game/input/ia_additionalSetting.ia_additionalSetting"));
	if (IA_ADDITIONALSETTING.Succeeded())
		additionalSettingAction = IA_ADDITIONALSETTING.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_AIM
	(TEXT("/Game/input/ia_aim.ia_aim"));
	if (IA_AIM.Succeeded())
		aimAction = IA_AIM.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_CANCEL
	(TEXT("/Game/input/ia_cancel.ia_cancel"));
	if (IA_CANCEL.Succeeded())
		cancelAction = IA_CANCEL.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_CHAT
	(TEXT("/Game/input/ia_chat.ia_chat"));
	if (IA_CHAT.Succeeded())
		chatAction = IA_CHAT.Object;
}

void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ADPCharacter로 캐스팅, 현재 Pawn을 character 변수에 할당
	character = Cast<ADPCharacter>(GetPawn());

	// character가 유효한지 확인
	if (!character) {
		UE_LOG(LogTemp, Warning, TEXT("character null"));
		return;
	}

	// subsystem, IMC 연결
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		SubSystem->AddMappingContext(defaultContext, 0);
}

void ADPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// enhanced input component 캐스팅하고 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		// 플레이어 이동 ( w, a, d, s )
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Move);
		// 플레이어 점프 ( space )
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Jump);
		// 시점 변환 ( 마우스 회전 )
		EnhancedInputComponent->BindAction(rotateAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Rotate);
		//	행동, 총 발사/벽 설치/터렛 설치 ( 마우스 좌클릭 )
		EnhancedInputComponent->BindAction(activeAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Active);
		//	변경, 무기 변경/벽 회전 ( 마우스 스크롤 )
		EnhancedInputComponent->BindAction(additionalSettingAction, ETriggerEvent::Triggered, this, &ADPPlayerController::AdditionalSetting);
		//	에임 ( 마우스 우클릭 )
		EnhancedInputComponent->BindAction(aimAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Aim);	// 	key down
		EnhancedInputComponent->BindAction(aimAction, ETriggerEvent::Completed, this, &ADPPlayerController::AimReleased);
		//	취소, 채팅 끄기 ( esc - UE 에디터에서 기본 단축키 변경 필요 )
		EnhancedInputComponent->BindAction(cancelAction, ETriggerEvent::Triggered, this, &ADPPlayerController::ActionCancel);
		//	채팅 열기 ( enter )
		EnhancedInputComponent->BindAction(chatAction, ETriggerEvent::Triggered, this, &ADPPlayerController::OpenChat);
	}
}

void ADPPlayerController::Move(const FInputActionValue& value)
{
	// UE_LOG(LogTemp, Warning, TEXT("ia_move_x : %f"), value.Get<FVector2D>().X);
	// UE_LOG(LogTemp, Warning, TEXT("ia_move_y : %f"), value.Get<FVector2D>().Y);

	const FVector2D actionValue = value.Get<FVector2D>();
	const FRotator controlRotation = GetControlRotation();
	const FRotator yaw(0.f, controlRotation.Yaw, 0.f);

	const FVector forwardVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::Y);
	
	// send move command ( id, actionValue ) ( x = 1 forward, x = -1 backward, y = 1 right, y = -1 left )
	character->AddMovementInput(forwardVector, actionValue.X);
	character->AddMovementInput(rightVector, actionValue.Y);

	//FVector preLoc = character->GetActorLocation();
	//if (1 == actionValue.X)
	//{
	//	FVector nextLoc;
	//	nextLoc.X = preLoc.X + 10.f;
	//	nextLoc.Y = preLoc.Y;
	//	nextLoc.Z = preLoc.Z;
	//	character->SetActorLocation(nextLoc);

	//	//character->currentVelocity = character->GetCharacterMovement()->Velocity;
	//	//speed = currentVelocity.Size();
	//	UE_LOG(LogTemp, Warning, TEXT("speed : %f"), character->speed);
	//}
	
	/*
	클라 w 키를 눌러 -> 속도, ( 어색하면 blend space에 범위로 )
	서버 location ( )
	*/

}

void ADPPlayerController::Jump(const FInputActionValue& value)
{
	//UE_LOG(LogTemp, Warning, TEXT("ia_jump : %d"), value.Get<bool>());
	
	bool actionValue = value.Get<bool>();
	if (actionValue)
		// send jump command ( id, actionValue ) ( true = jump )
		character->Jump();
}

void ADPPlayerController::Rotate(const FInputActionValue& value)
{
	// UE_LOG(LogTemp, Warning, TEXT("ia_rotate_X : %f"), value.Get<FVector2D>().X);
	// UE_LOG(LogTemp, Warning, TEXT("ia_rotate_y : %f"), value.Get<FVector2D>().Y);
	
	FVector2D actionValue = value.Get<FVector2D>();

	// send rotate command ( id, actionValue )
	character->AddControllerYawInput(actionValue.X);
	character->AddControllerPitchInput(actionValue.Y);
}

void ADPPlayerController::Active(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Active"));

	// if (state)
	character->weaponComponent->Attack();
}

void ADPPlayerController::AdditionalSetting(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("AdditionalSetting"));
	// Scroll Up : 1, Scroll Down : -1
	UE_LOG(LogTemp, Warning, TEXT("->	ia_rotate_x : %f"), value.Get<FVector2D>().X);
}

void ADPPlayerController::Aim(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Aim"));
}

void ADPPlayerController::AimReleased(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("AimReleased"));
}

void ADPPlayerController::ActionCancel(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("ActionCancel"));
}

void ADPPlayerController::OpenChat(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("OpenChat"));
}


void ADPPlayerController::UpdatePlayer(/*DataHub result*/)	// 한번에 받기 ?
{
	/*
	if (result.moveResult)
		character->SetActorLocation(moveResult.x, moveResult.y, moveResult.z);
	if (result.jumpResult)
		character->Jump();
	if (result.rotateResult)
		character->
	character->SetActorRotation();
	character->SetActorLocationAndRotation();
	*/
}
