#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/ScoreTypes.h"
#include "CalculateScoreByJobs.generated.h"

UCLASS( ClassGroup=(Score), meta=(BlueprintSpawnableComponent) )
class PROJ_A_API UCalculateScoreByJobs : public UActorComponent
{
	GENERATED_BODY()

public:
	UCalculateScoreByJobs();
	static int CalculateScoreByJobs(EPlayerJob Job, const TArray<EAnimal>& animals);
private:
	static int CalculateHunter( const TArray<EAnimal>& animals);
	static int CalculateFarmer( const TArray<EAnimal>& animals);
};
