#pragma once

#include "CoreMinimal.h"
#include "BaseInputComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "MainInputComponent.generated.h"

UCLASS()
class PROJ_A_API UMainInputComponent : public UBaseInputComponent
{
	GENERATED_BODY()
public:
	UMainInputComponent();
	
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;

private:
	bool IsValidateToRunThisFunction();
	void BindMainLevelActions();
	void UnbindMainLevelActions();
	
	void Move(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);
	void Active(const FInputActionValue& value);
	void AdditionalSetting(const FInputActionValue& value);
	void Aim(const FInputActionValue& value);
	void AimReleased(const FInputActionValue& value);
	void ActionCancel(const FInputActionValue& value);
	void CatchAnimals(const FInputActionValue& value);
	void ReturningAnimals(const FInputActionValue& value);
	void Run(const FInputActionValue& value);
	void RunReleased(const FInputActionValue& value);
	void Esc(const FInputActionValue& value);

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* MainLevelContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* MoveAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* JumpAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RotateAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ActiveAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* AdditionalSettingAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* AimAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* CancelAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* EscAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* CatchAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ReturnAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RunAction;
};
