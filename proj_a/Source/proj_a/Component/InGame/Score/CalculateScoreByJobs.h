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
	static FScoreData CalculateScoreByJobs(EPlayerJob Job, const TArray<EAnimal>& animals);
private:
	static FScoreData CalculateArchaeologist( const TArray<EAnimal>& animals);
	static FScoreData CalculatePoacher( const TArray<EAnimal>& animals);
	static FScoreData CalculateRingmaster( const TArray<EAnimal>& animals);
	static FScoreData CalculateTerrorist( const TArray<EAnimal>& animals);
	static FScoreData CalculateEnvironmentalist( const TArray<EAnimal>& animals);
	static bool AnimalCombinationCheck ( const TArray<EAnimal>& animals, const TArray<EAnimal>& combination);
};
