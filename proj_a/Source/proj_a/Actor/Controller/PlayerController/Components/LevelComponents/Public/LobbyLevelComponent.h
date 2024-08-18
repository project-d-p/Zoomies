#pragma once

#include "CoreMinimal.h"
#include "BaseLevelComponent.h"
#include "LobbyLevelComponent.generated.h"

UCLASS()
class ULobbyLevelComponent : public UBaseLevelComponent
{
	GENERATED_BODY()
public:
	ULobbyLevelComponent();

	/* Activate and Deactivate this Component */
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
