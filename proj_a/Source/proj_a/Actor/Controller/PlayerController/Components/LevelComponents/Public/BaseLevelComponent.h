#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseInputComponent.h"
#include "SoundComponent.h"
#include "BaseLevelComponent.generated.h"

UCLASS(Abstract)
class PROJ_A_API UBaseLevelComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	ADPPlayerController* GetPlayerController() const;
	APawn* GetPlayerCharacter();
	
	UPROPERTY()
	UBaseInputComponent* InputComponent;
	UPROPERTY()
	USoundComponent* SoundComponent;
	
	ADPPlayerController* CachedPlayerController;
	APawn* CachedCharacter;
	
public:
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;

	
	void InitializeController(ADPPlayerController* AdpPlayerController);
	
	UBaseInputComponent* GetInputComponent() const;
	USoundComponent* GetSoundComponent() const;
};
