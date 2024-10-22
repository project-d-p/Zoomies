#include "BasicInputComponent.h"

#include "DPPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

UBasicInputComponent::UBasicInputComponent()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		BasicLevelContext = DEFAULT_CONTEXT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_MOVE
	(TEXT("/Game/input/ia_move.ia_move"));
	if (IA_MOVE.Succeeded())
		MoveAction = IA_MOVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_JUMP
	(TEXT("/Game/input/ia_jump.ia_jump"));
	if (IA_JUMP.Succeeded())
		JumpAction = IA_JUMP.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ROTATE
	(TEXT("/Game/input/ia_rotate.ia_rotate"));
	if (IA_ROTATE.Succeeded())
		RotateAction = IA_ROTATE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_RUN
	(TEXT("/Game/input/ia_run.ia_run"));
	if (IA_RUN.Succeeded())
		RunAction = IA_RUN.Object;
}

void UBasicInputComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	BindBasicLevelActions();
}

void UBasicInputComponent::Deactivate()
{
	Super::Deactivate();
	UnbindBasicLevelActions();
}

void UBasicInputComponent::BindBasicLevelActions()
{
	ADPPlayerController* PlayerController = GetPlayerController();
	if (!PlayerController) return ;
	if (!PlayerController->IsLocalController()) return ;

	// Mapping Context 추가
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(BasicLevelContext, 0);
	}

	// Enhanced Input Component 가져오기 및 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		// 플레이어 이동 ( w, a, d, s )
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UBasicInputComponent::Move);
		// 플레이어 점프 ( space )
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UBasicInputComponent::Jump);
		// 시점 변환 ( 마우스 회전 )
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &UBasicInputComponent::Rotate);
		// 달리기 ( shift )
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &UBasicInputComponent::Run);
	}
}

void UBasicInputComponent::UnbindBasicLevelActions()
{
	ADPPlayerController* PlayerController = GetPlayerController();
	if (PlayerController) return ;

	// Enhanced Input Component 바인딩 제거
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();
	}

	// Mapping Context 제거
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (BasicLevelContext)
			SubSystem->RemoveMappingContext(BasicLevelContext);
	}
}

void UBasicInputComponent::Move(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	if (Character->IsStunned())
	{
		return ;
	}
	const FVector2D actionValue = value.Get<FVector2D>();
	const FRotator controlRotation = PlayerController->GetControlRotation();

	const FVector forwardVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::Y);

	Character->AddMovementInput(forwardVector, actionValue.X);
	Character->AddMovementInput(rightVector, actionValue.Y);
}

void UBasicInputComponent::Jump(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	if (Character->IsStunned())
	{
		return ;
	}
	Character->Jump();
	
	USoundComponent* SoundComponent = OwningLevelComponent->GetSoundComponent();
	if (SoundComponent)
		SoundComponent->PlayJumpSound();
}

void UBasicInputComponent::Rotate(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	
	FVector2D actionValue = value.Get<FVector2D>();

	Character->AddControllerYawInput(actionValue.X);
	Character->AddControllerPitchInput(actionValue.Y);
}

void UBasicInputComponent::Run(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;

	Character->GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}

void UBasicInputComponent::Set_PC(ADPPlayerController* PlayerController)
{
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::PlayerController is nullptr."));
	}
	PC = PlayerController;
}