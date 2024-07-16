#include "BaseLevelComponent.h"

#include "DPPlayerController.h"

ADPPlayerController* UBaseLevelComponent::GetPlayerController() const
{
	return CachedPlayerController;
}

APawn* UBaseLevelComponent::GetPlayerCharacter()
{
	if (CachedCharacter == nullptr)
	{
		if (CachedPlayerController != nullptr)
			CachedCharacter = CachedPlayerController->GetPawn();
	}
	return CachedCharacter;
}

void UBaseLevelComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void UBaseLevelComponent::Deactivate()
{
	Super::Deactivate();
}

void UBaseLevelComponent::InitializeController(ADPPlayerController* AdpPlayerController)
{
	CachedPlayerController = AdpPlayerController;

	InputComponent->InitializeController(AdpPlayerController);
}

UBaseInputComponent* UBaseLevelComponent::GetInputComponent() const
{
	return InputComponent;
}

USoundComponent* UBaseLevelComponent::GetSoundComponent() const
{
	return SoundComponent;
}
