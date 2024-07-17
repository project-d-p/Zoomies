#include "BaseInputComponent.h"

#include "DPPlayerController.h"

void UBaseInputComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void UBaseInputComponent::Deactivate()
{
	Super::Deactivate();
}

void UBaseInputComponent::InitializeController(ADPPlayerController* AdpPlayerController)
{
	CachedPlayerController = AdpPlayerController;
}

void UBaseInputComponent::SetLevelComponent(UBaseLevelComponent* MainLevelComponent)
{
	OwningLevelComponent = MainLevelComponent;
}

void UBaseInputComponent::SetPlayerCharacter(APawn* Pawn)
{
	CachedCharacter = Pawn;
}

ADPPlayerController* UBaseInputComponent::GetPlayerController() const
{
	return CachedPlayerController;
}

APawn* UBaseInputComponent::GetPlayerCharacter()
{
	if (CachedCharacter == nullptr)
	{
		if (CachedPlayerController != nullptr)
			CachedCharacter = CachedPlayerController->GetPawn();
	}
	return CachedCharacter;
}
