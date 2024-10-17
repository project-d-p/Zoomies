#include "JudgeInputComponent.h"

#include "DPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "JudgePlayerController.h"
#include "JudgeLevelComponent.h"

UJudgeInputComponent::UJudgeInputComponent()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		LobbyLevelContext = DEFAULT_CONTEXT.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ACTIVE
	(TEXT("/Game/input/ia_active.ia_active"));
	if (IA_ACTIVE.Succeeded())
		ActiveAction = IA_ACTIVE.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ESC
	(TEXT("/Game/input/ia_esc.ia_esc"));
	if (IA_ESC.Succeeded())
		EscAction = IA_ESC.Object;
}

void UJudgeInputComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	BindJudgeLevelActions();
}

void UJudgeInputComponent::Deactivate()
{
	Super::Deactivate();
	UnbindJudgeLevelActions();
}

void UJudgeInputComponent::Set_PC(AJudgePlayerController* Apc_MatchingLobby)
{
	if (Apc_MatchingLobby == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::PlayerController is nullptr."));
	}
	PC_JudgeLevel = Apc_MatchingLobby;
}

void UJudgeInputComponent::BindJudgeLevelActions()
{
	AJudgePlayerController* PlayerController = PC_JudgeLevel;
	if (!PlayerController) return ;
	if (!PlayerController->IsLocalController()) return ;

	// Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(LobbyLevelContext, 0);
	}

	// Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->BindAction(ActiveAction, ETriggerEvent::Triggered, this, &UJudgeInputComponent::Active);
		EnhancedInputComponent->BindAction(EscAction, ETriggerEvent::Started, this, &UJudgeInputComponent::Esc);
	}
}

void UJudgeInputComponent::UnbindJudgeLevelActions()
{
	AJudgePlayerController* PlayerController = PC_JudgeLevel;
	if (!PlayerController) return ;

	// Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();
	}

	// Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (LobbyLevelContext)
		{
			SubSystem->RemoveMappingContext(LobbyLevelContext);
		}
	}
}

void UJudgeInputComponent::Active(const FInputActionValue& value)
{
	return ;
}

void UJudgeInputComponent::Esc(const FInputActionValue& value)
{
	AJudgePlayerController* PlayerController = Cast<AJudgePlayerController>(PC_JudgeLevel);
	if (PlayerController != nullptr)
	{
		PlayerController->ShowUI_ESC();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMainInputComponent::Esc::PlayerController is nullptr."));
	}
}

void UJudgeInputComponent::Set_LC(UJudgeLevelComponent* LC_JudgeLevel)
{
	if (LC_JudgeLevel == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IC_MatchLobby::LevelComponent is nullptr."));
	}
	OwningLevelComponent = LC_JudgeLevel;
}