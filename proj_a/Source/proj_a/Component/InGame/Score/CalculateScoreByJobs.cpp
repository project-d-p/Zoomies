#include "CalculateScoreByJobs.h"

UCalculateScoreByJobs::UCalculateScoreByJobs()
{
}

FScoreData UCalculateScoreByJobs::CalculateScoreByJobs(EPlayerJob Job, const TArray<EAnimal>& animals)
{
	FScoreData score = {0, 0, 1};
	
	switch (Job)
	{
		case EPlayerJob::JOB_ARCHAEOLOGIST:
			score = CalculateArchaeologist(animals);
			break;
		case EPlayerJob::JOB_POACHER
			score = CalculatePoacher(animals);
			break;
		case EPlayerJob::JOB_RINGMASTER:
			score = CalculateRingmaster(animals);
			break;
		case EPlayerJob::JOB_TERRORIST:
			score = CalculateTerrorist(animals);
			break;
		case EPlayerJob::JOB_ENVIRONMENTALIST:
			score = CalculateEnvironmentalist(animals);
			break;
		default:
			break;
	}
	
	return score;
}
FScoreData UCalculateScoreByJobs::CalculateArchaeologist( const TArray<EAnimal>& animals)
{
	FScoreData score = {0, 0, 1};
	//iterate animals
	for (const EAnimal& animal: animals)
	{
		
	}
	return score;
}

FScoreData UCalculateScoreByJobs::CalculatePoacher( const TArray<EAnimal>& animals);
{
	FScoreData score = {0, 0, 1};
	
	return score;
}

FScoreData UCalculateScoreByJobs::CalculateRingmaster( const TArray<EAnimal>& animals);
{
	FScoreData score = {0, 0, 1};
	return score;
}

FScoreData UCalculateScoreByJobs::CalculateTerrorist( const TArray<EAnimal>& animals);
{
	FScoreData score = {0, 0, 1};
	return score;
}

FScoreData UCalculateScoreByJobs::CalculateEnvironmentalist( const TArray<EAnimal>& animals);
{
	FScoreData score = {0, 0, 1};
	return score;
}

bool UCalculateScoreByJobs::AnimalCombinationCheck ( const TArray<EAnimal>& animals, const TArray<EAnimal>& combination)
{
	for (const EAnimal& animal: combination)
	{
		if (!animals.Contains(animal))
		{
			return false;
		}
	}
	return true;	
}