#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Types/ScoreTypes.h"
#include "ScoreManager.generated.h"

UCLASS()
class PROJ_A_API UScoreManagerComp : public UActorComponent
{
	GENERATED_BODY()

public:    
	UScoreManagerComp();
	
	void IncreasePlayerScore(APlayerController* PlayerController, const TArray<EAnimal>& Animals);
protected:
	virtual void BeginPlay() override;
};
