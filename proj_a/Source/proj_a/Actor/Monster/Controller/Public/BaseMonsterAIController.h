#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseMonsterAIController.generated.h"

UCLASS()
class PROJ_A_API ABaseMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseMonsterAIController();

	UFUNCTION()
	UAISightComponent* GetAISightComponent() const;
	
	void MoveToTargetLocation();

private:
	UPROPERTY()
	UAISightComponent* AISightComponent = nullptr;
	
protected:
	UFUNCTION()
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void Tick(float DeltaSeconds) override;
	// virtual void OnPossess(APawn* InPawn) override;
	// virtual void OnUnPossess() override;
};
