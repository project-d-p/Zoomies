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

private:
	UPROPERTY()
	UAISightComponent* AISightComponent = nullptr;
protected:
	virtual void BeginPlay() override;
	// virtual void OnPossess(APawn* InPawn) override;
	// virtual void OnUnPossess() override;
};
