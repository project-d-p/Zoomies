#include "LobbyInputComponent.h"
#include "DPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "FNetLogger.h"
#include "MainInputComponent.h"

ULobbyInputComponent::ULobbyInputComponent()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		LobbyLevelContext = DEFAULT_CONTEXT.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ACTIVE
	(TEXT("/Game/input/ia_active.ia_active"));
	if (IA_ACTIVE.Succeeded())
		ActiveAction = IA_ACTIVE.Object;
}

void ULobbyInputComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	BindMainLevelActions();
}

void ULobbyInputComponent::Deactivate()
{
	Super::Deactivate();
	UnbindMainLevelActions();
}

void ULobbyInputComponent::BindMainLevelActions()
{
	ADPPlayerController* PlayerController = GetPlayerController();
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
		// (left mouse button) : Active
		EnhancedInputComponent->BindAction(ActiveAction, ETriggerEvent::Triggered, this, &ULobbyInputComponent::Active);
	}
}

void ULobbyInputComponent::UnbindMainLevelActions()
{
	ADPPlayerController* PlayerController = GetPlayerController();
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

void ULobbyInputComponent::Active(const FInputActionValue& value)
{
	return ;
}
