#pragma once

#include "CoreMinimal.h"
#include "DPCharacter.h"
#include "BaseInputComponent.generated.h"

class UBaseLevelComponent;
class ADPPlayerController;
class ADPCharacter;

UCLASS(Abstract)
class PROJ_A_API UBaseInputComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	
	void InitializeController(ADPPlayerController* AdpPlayerController);
	void SetLevelComponent(UBaseLevelComponent* MainLevelComponent);

protected:
	ADPPlayerController* GetPlayerController() const;
	APawn* GetPlayerCharacter();
	
	ADPPlayerController* CachedPlayerController = nullptr;
	APawn* CachedCharacter = nullptr;

	UPROPERTY()
	class UBaseLevelComponent* OwningLevelComponent = nullptr;
};
