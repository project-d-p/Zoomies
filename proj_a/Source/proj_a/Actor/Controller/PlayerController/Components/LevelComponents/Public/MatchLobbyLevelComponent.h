#pragma once

#include "CoreMinimal.h"
#include "BaseLevelComponent.h"
#include "MatchLobbyLevelComponent.generated.h"

UCLASS()
class UMatchLobbyLevelComponent : public UBaseLevelComponent
{
	GENERATED_BODY()
public:
	UMatchLobbyLevelComponent();

	UFUNCTION(BlueprintCallable)
	void SetFixedCameraView();
	
	/* Activate and Deactivate this Component */
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
