#include "CalculateScoreByJobs.h"

UCalculateScoreByJobs::UCalculateScoreByJobs()
{
}

int UCalculateScoreByJobs::CalculateScoreByJobs(EPlayerJob Job, const TArray<EAnimal>& animals)
{
	int score = 0;
	
	switch (Job)
	{
		case EPlayerJob::JOB_FARMER:
			score += CalculateFarmer(animals);
			break;
		case EPlayerJob::JOB_HUNTER:
			score += CalculateHunter(animals);
			break;
		default:
			break;
	}
	
	return score;
}

int UCalculateScoreByJobs::CalculateHunter(const TArray<EAnimal>& animals)
{
	int score = 0;
	
	for (EAnimal animal : animals)
	{
		score += 1;
		if (animal == EAnimal::ANIMAL_DEER)
		{
			score += 10;
		}
	}
	
	return score;
}

int UCalculateScoreByJobs::CalculateFarmer(const TArray<EAnimal>& animals)
{
	int score = 0;
	
	for (EAnimal animal : animals)
	{
		score += 1;
		if (animal == EAnimal::ANIMAL_COW)
		{
			score += 10;
		}
	}
	
	return score;
}
