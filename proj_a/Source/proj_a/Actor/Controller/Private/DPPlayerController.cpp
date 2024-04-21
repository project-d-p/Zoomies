// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"
#include "DPCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "NetComp.h"
#include "ProtobufUtility.h"
#include "movement.pb.h"

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
}

void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ADPCharacter�� ĳ����, ���� Pawn�� character ������ �Ҵ�
	character = Cast<ADPCharacter>(GetPawn());

	// character�� ��ȿ���� Ȯ��
	if (!character) {
		UE_LOG(LogTemp, Warning, TEXT("character null"));
		return;
	}

	// subsystem, IMC ����
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		SubSystem->AddMappingContext(defaultContext, 0);
}

void ADPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// enhanced input component ĳ�����ϰ� ���ε�
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		// �÷��̾� �̵� ( w, a, d, s )
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Move);
		// �÷��̾� ���� ( space )
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Jump);
		// ���� ��ȯ ( ���콺 ȸ�� )
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

	UNetComp::inputTCP(actionValue, 0);
	// send move command ( id, actionValue ) ( x = 1 forward, x = -1 backward, y = 1 right, y = -1 left )
	character->AddMovementInput(forwardVector, actionValue.X);
	character->AddMovementInput(rightVector, actionValue.Y);
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
	ProtoData.set_allocated_orientation(ProtobufUtility::ConvertToFVecToVec3(character->GetControlRotation().Vector()));
}

void ADPPlayerController::UpdatePlayer(/*DataHub result*/)	// �ѹ��� �ޱ� ?
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
