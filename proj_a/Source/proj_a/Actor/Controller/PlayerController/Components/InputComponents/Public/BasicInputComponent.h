#pragma once

#include "CoreMinimal.h"
#include "BaseInputComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "BasicInputComponent.generated.h"

UCLASS()
class UBasicInputComponent : public UBaseInputComponent
{
	GENERATED_BODY()
public:
	UBasicInputComponent();

	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	void Set_PC(ADPPlayerController* PlayerController);
	
private:
	void BindBasicLevelActions();
	void UnbindBasicLevelActions();

	void Move(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);
	void Run(const FInputActionValue& value);
	void RunReleased(const FInputActionValue& value);

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* BasicLevelContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* MoveAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* JumpAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RotateAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RunAction;


protected:
	UPROPERTY()
	ADPPlayerController* PC = nullptr;
};
