#pragma once

#include "CoreMinimal.h"
#include "BaseLevelComponent.h"
#include "ResultLevelComponent.generated.h"

UCLASS()
class UResultLevelComponent : public UBaseLevelComponent
{
	GENERATED_BODY()
public:
	UResultLevelComponent();

	UFUNCTION(BlueprintCallable)
	void SetFixedCameraView();
	
	/* Activate and Deactivate this Component */
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	ADPPlayerController* PC = nullptr;
	void Set_PC(ADPPlayerController* PlayerController);
};
