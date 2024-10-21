#include "IC_MatchLobby.h"

#include "DPPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CineCameraActor.h"
#include "../LevelComponents/LC_MatchLobby.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"

UIC_MatchLobby::UIC_MatchLobby()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		InputMappingContext = DEFAULT_CONTEXT.Object;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("IC_MatchLobby::Failed to load InputMappingContext"));
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_MOVE
	(TEXT("/Game/input/ia_move.ia_move"));
	if (IA_MOVE.Succeeded())
		MoveAction = IA_MOVE.Object;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("IC_MatchLobby::Failed to load MoveAction"));
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_JUMP
	(TEXT("/Game/input/ia_jump.ia_jump"));
	if (IA_JUMP.Succeeded())
		JumpAction = IA_JUMP.Object;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("IC_MatchLobby::Failed to load JumpAction"));
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ROTATE
	(TEXT("/Game/input/ia_rotate.ia_rotate"));
	if (IA_ROTATE.Succeeded())
		RotateAction = IA_ROTATE.Object;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("IC_MatchLobby::Failed to load RotateAction"));
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_INTERACT
	(TEXT("/Game/input/ia_interact.ia_interact"));
	if (IA_INTERACT.Succeeded())
		InteractAction = IA_INTERACT.Object;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("IC_MatchLobby::Failed to load InteractAction"));
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ESC
	(TEXT("/Game/input/ia_ESC.ia_ESC"));
	if (IA_ESC.Succeeded())
		ESCAction = IA_ESC.Object;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("IC_MatchLobby::Failed to load EscAction"));
	}
}

void UIC_MatchLobby::Activate(bool bReset)
{
	Super::Activate(bReset);
	BindMatchLobbyActions();
}

void UIC_MatchLobby::Deactivate()
{
	Super::Deactivate();
	UnbindMatchLobbyActions();
}

void UIC_MatchLobby::BindMatchLobbyActions()
{
	APC_MatchingLobby* PlayerController = Get_PC();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found PlayerController to Bind Input."));
		return ;
	}
	if (!PlayerController->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found Local PlayerController"));
		return ;
	}

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(InputMappingContext, 0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found EnhancedInputLocalPlayerSubsystem"));
		return ;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UIC_MatchLobby::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UIC_MatchLobby::Jump);
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &UIC_MatchLobby::Rotate);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UIC_MatchLobby::Interact);
		EnhancedInputComponent->BindAction(ESCAction, ETriggerEvent::Started, this, &UIC_MatchLobby::ESC);
		if (!MoveAction || !JumpAction || !RotateAction)
		{
			UE_LOG(LogTemp, Error, TEXT("IC_MatchLobby::One or more InputActions are not valid"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::InputActions are valid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found EnhancedInputComponent"));
	}
}

void UIC_MatchLobby::UnbindMatchLobbyActions()
{
	APC_MatchingLobby* PlayerController = Get_PC(); 
	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found PlayerController to Unbind Input."));
		return ;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found EnhancedInputComponent"));
		return ;
	}

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (InputMappingContext)
			SubSystem->RemoveMappingContext(InputMappingContext);
		else 
			UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found InputMappingContext"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found EnhancedInputLocalPlayerSubsystem"));
	}
}

void UIC_MatchLobby::Move(const FInputActionValue& value)
{
	APC_MatchingLobby* PlayerController = Get_PC(); 
	ADPCharacter* Character = Cast<ADPCharacter>(Get_CHAR());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found PlayerCharacter"));
		return ;
	}
	if (Character->IsStunned())
	{
		return ;
	}
	const FVector2D actionValue = value.Get<FVector2D>();
	const FRotator controlRotation = PlayerController->FixedCamera ? PlayerController->FixedCamera->GetActorRotation() : PlayerController->GetControlRotation();

	const FVector forwardVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::Y);

	Character->AddMovementInput(forwardVector, actionValue.X);
	Character->AddMovementInput(rightVector, actionValue.Y);
}

void UIC_MatchLobby::Jump(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(Get_CHAR());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found PlayerCharacter"));
		return ;
	}
	if (Character->IsStunned())
	{
		return ;
	}
	Character->Jump();
}

void UIC_MatchLobby::Rotate(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(Get_CHAR());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found PlayerCharacter"));
		return ;
	}
	
	FVector2D actionValue = value.Get<FVector2D>();

	Character->AddControllerYawInput(actionValue.X);
	Character->AddControllerPitchInput(actionValue.Y);
}

void UIC_MatchLobby::Interact(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(Get_CHAR());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Can't found PlayerCharacter"));
		return ;
	}
	if (Character->canInteract)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Interact function called."));
		if (PC_MatchLobby != nullptr)
		{
			PC_MatchLobby->UpdateUIVisibility();
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT("IC_MatchLobby::PlayerController is nullptr."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Interact function called but canInteract is false."));
	}
}

void UIC_MatchLobby::ESC(const FInputActionValue& value)
{
	if (PC_MatchLobby != nullptr)
	{
		PC_MatchLobby->ShowUI_ESC();
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT("IC_MatchLobby::PlayerController is nullptr."));
	}
}

APC_MatchingLobby* UIC_MatchLobby::Get_PC() const
{
	return PC_MatchLobby;
}

APawn* UIC_MatchLobby::Get_CHAR()
{
	if (PC_MatchLobby != nullptr)
	{
		CHAR_MatchLobby = PC_MatchLobby->GetPawn();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Charactor is nullptr."));
	}
	return CHAR_MatchLobby;
}

void UIC_MatchLobby::Set_PC(APC_MatchingLobby* Apc_MatchingLobby)
{
	if (Apc_MatchingLobby == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::PlayerController is nullptr."));
	}
	PC_MatchLobby = Apc_MatchingLobby;
}

void UIC_MatchLobby::Set_CHAR(APawn* Pawn)
{
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::Pawn is nullptr."));
	}
	CHAR_MatchLobby = Pawn;
}

void UIC_MatchLobby::Set_LC(ULC_MatchLobby* Lc_MatchLobby)
{
	if (Lc_MatchLobby == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::LevelComponent is nullptr."));
	}
	OwningLevelComponent = Lc_MatchLobby;
}

