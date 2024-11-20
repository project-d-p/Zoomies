#pragma once

#include "JudgePlayerController.h"
#include "JudgeLevelComponent.generated.h"

class UJudgeInputComponent;

UCLASS()
class PROJ_A_API UJudgeLevelComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UJudgeLevelComponent();

	/* Activate and Deactivate this Component */
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	
	void Set_PC(AJudgePlayerController* AdpPlayerController);
	UJudgeInputComponent* GetInputComponent() const;
	
protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	UJudgeInputComponent* InputComponent ;
	
private:
	bool IsBeginPlay = false;
	FTimerHandle TH;
	UPROPERTY()
	AJudgePlayerController* PC_JudegeLevel = nullptr;
};
