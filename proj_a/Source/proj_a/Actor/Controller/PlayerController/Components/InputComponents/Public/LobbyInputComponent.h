#pragma once

#include "CoreMinimal.h"
#include "BaseInputComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "LobbyInputComponent.generated.h"

UCLASS()
class ULobbyInputComponent : public UBaseInputComponent
{
	GENERATED_BODY()
public:
	ULobbyInputComponent();

	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	
private:
	void BindMainLevelActions();
	void UnbindMainLevelActions();
	void Active(const FInputActionValue& value);
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* LobbyLevelContext;
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ActiveAction;
};
