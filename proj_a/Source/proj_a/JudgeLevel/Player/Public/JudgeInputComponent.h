#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "JudgeInputComponent.generated.h"

class UJudgeLevelComponent;
class AJudgePlayerController;

UCLASS()
class UJudgeInputComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UJudgeInputComponent();

	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	
	void Set_PC(AJudgePlayerController* Apc_MatchingLobby);
	void Set_LC(UJudgeLevelComponent* LC_JudegeLevel);
	
private:
	void BindJudgeLevelActions();
	void UnbindJudgeLevelActions();
	
	void Active(const FInputActionValue& value);
	void Esc(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* LobbyLevelContext;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ActiveAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* EscAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RotateAction;
	
protected:
	UPROPERTY()
	AJudgePlayerController* PC_JudgeLevel = nullptr;
	
	UPROPERTY()
	class UJudgeLevelComponent* OwningLevelComponent = nullptr;
};
