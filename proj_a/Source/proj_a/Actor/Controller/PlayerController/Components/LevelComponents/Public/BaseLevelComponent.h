#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseInputComponent.h"
#include "SoundComponent.h"
#include "ChatManager.h"
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
	UChatManager* ChatManager;
	UPROPERTY()
	USoundComponent* SoundComponent;

	UPROPERTY()
	ADPPlayerController* CachedPlayerController;
	UPROPERTY()
	APawn* CachedCharacter;
	
public:
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	
	void InitializeController(ADPPlayerController* AdpPlayerController);
	void SetPlayerCharacter(APawn* InCharacter);
	
	UBaseInputComponent* GetInputComponent() const;
	USoundComponent* GetSoundComponent() const;
};
