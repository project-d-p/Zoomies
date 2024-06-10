#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DPPlayerState.generated.h"

UCLASS()
class PROJ_A_API ADPPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADPPlayerState();
	
	UFUNCTION()
	UPlayerScoreComp* GetPlayerScoreComp() const;
private:
	UPROPERTY()
	UPlayerScoreComp* PlayerScoreComp = nullptr;

	virtual void BeginPlay() override;
	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
