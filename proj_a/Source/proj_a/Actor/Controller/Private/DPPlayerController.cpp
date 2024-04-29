// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"

#include "DPCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "NetComp.h"
#include "ProtobufUtility.h"
#include "movement.pb.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "FUdpSendTask.h"

DEFINE_LOG_CATEGORY(LogNetwork);

ADPPlayerController::ADPPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DEFAULT_CONTEXT
		(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		defaultContext = DEFAULT_CONTEXT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MOVE
		(TEXT("/Game/input/ia_move.ia_move"));
	if (IA_MOVE.Succeeded())
		moveAction = IA_MOVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_JUMP
		(TEXT("/Game/input/ia_jump.ia_jump"));
	if (IA_JUMP.Succeeded())
		jumpAction = IA_JUMP.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ROTATE
		(TEXT("/Game/input/ia_rotate.ia_rotate"));
	if (IA_ROTATE.Succeeded())
		rotateAction = IA_ROTATE.Object;
}

void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ADPCharacter로 캐스팅, 현재 Pawn을 character 변수에 할당
	character = Cast<ADPCharacter>(GetPawn());

	// character가 유효한지 확인
	if (!character)
	{
		UE_LOG(LogTemp, Warning, TEXT("character null"));
		return;
	}

	// subsystem, IMC 연결
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
		SubSystem->AddMappingContext(defaultContext, 0);
}

void ADPPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdatePlayer();
}

void ADPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// enhanced input component 캐스팅하고 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 플레이어 이동 ( w, a, d, s )
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Move);
		// 플레이어 점프 ( space )
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Jump);
		// 시점 변환 ( 마우스 회전 )
		EnhancedInputComponent->BindAction(rotateAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Rotate);
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

	// XXX: 2번째 인자인 type은 나중에 사용, 0이 기본 이동 처리.
	// UNetComp::inputTCP(actionValue, 0);
	UNetComp::InputUDP(actionValue);
	// character->AddMovementInput(forwardVector, actionValue.X);
	// character->AddMovementInput(rightVector, actionValue.Y);
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
	// UE_LOG(LogTemp, Warning, TEXT("ia_rotate_x : %f"), value.Get<FVector2D>().X);
	// UE_LOG(LogTemp, Warning, TEXT("ia_rotate_y : %f"), value.Get<FVector2D>().Y);

	FVector2D actionValue = value.Get<FVector2D>();

	// send rotate command ( id, actionValue )
	character->AddControllerYawInput(actionValue.X);
	character->AddControllerPitchInput(actionValue.Y);
	// XXX: 추후에 최적화 가능(필요)
	FUdpSendTask::ProtoData.set_allocated_orientation(ProtobufUtility::ConvertToFVecToVec3(character->GetControlRotation().Vector()));
}

void ADPPlayerController::UpdatePlayer()
{
	Movement movement;
	if (!FDataHub::EchoData.Contains("1")) {
		// UE_LOG(LogNetwork, Warning, TEXT("Player 1 data not found"));
		return;
	}
	movement = FDataHub::EchoData["1"];
	if (!movement.has_progess_vector())
	{
		// UE_LOG(LogNetwork, Warning, TEXT("progress vector not found"));
	}
	// UE_LOG(LogTemp, Warning, TEXT("Progress: %f %f %f"), movement.progess_vector().x(), movement.progess_vector().y(), movement.progess_vector().z());
	if (movement.has_progess_vector())
	{
		FVector rightVector = character->GetActorRightVector();
		FVector forwardVector(movement.orientation().x(), movement.orientation().y(), movement.orientation().z());
		FVector actionValue = FVector(movement.progess_vector().x(), movement.progess_vector().y(), movement.progess_vector().z());
		
		character->AddMovementInput(forwardVector, actionValue.X);
		character->AddMovementInput(rightVector, actionValue.Y);
	}
	
	// class 속성으로 추가 고려.
	// PlayerPosition result = FDataHub::PlayerPositions["1"];
	// if (result.has_position())
	// 	character->SetActorLocation(FVector(result.position().x(), result.position().y(), result.position().z()));
	// if (result.jumpResult)
	// 	character->Jump();
	// if (result.rotateResult)
	// 	character->
	// character->SetActorRotation();
	// character->SetActorLocationAndRotation();
}
