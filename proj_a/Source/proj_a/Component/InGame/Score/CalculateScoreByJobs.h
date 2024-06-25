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
	static FScoreData CalScoreByJobs(
		const TArray<EAnimal>& animals,
		EAnimal favoriteAnimal,
		const TArray<EAnimal>& addCombine,
		const TArray<EAnimal>& mulCombine
		);
	static bool AnimalCombinationCheck ( const TArray<EAnimal>& animals, const TArray<EAnimal>& combination);
	static int CalBaseScore (const TArray<EAnimal>& animals, const EAnimal& favoriteAnimal);
};
