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
				score = CalScoreByJobs(
					animals,
					EAnimal::ANIMAL_MAMMOTH,
					{TArray<EAnimal>{
						EAnimal::ANIMAL_MAMMOTH,
						EAnimal::ANIMAL_CHAMELEON,
						EAnimal::ANIMAL_WHALE}},
					{TArray<EAnimal>{
						EAnimal::ANIMAL_MAMMOTH,
						EAnimal::ANIMAL_SABER_TOOTH_TIGER,
						EAnimal::ANIMAL_OCTOPUS,
						EAnimal::ANIMAL_GIRAFFE,
						EAnimal::ANIMAL_WHALE}});
				break;
		case EPlayerJob::JOB_POACHER:
				score = CalScoreByJobs(
					animals,
					EAnimal::ANIMAL_GIRAFFE,
					{TArray<EAnimal>{
						EAnimal::ANIMAL_GIRAFFE,
						EAnimal::ANIMAL_CHAMELEON,
						EAnimal::ANIMAL_HORSE
						}},
					{TArray<EAnimal>{
						EAnimal::ANIMAL_SABER_TOOTH_TIGER,
						EAnimal::ANIMAL_LION,
						EAnimal::ANIMAL_SLOTH,
						EAnimal::ANIMAL_GIRAFFE,
						EAnimal::ANIMAL_FOX}});
				break;
		case EPlayerJob::JOB_RINGMASTER:
			score = CalScoreByJobs(
				animals,
				EAnimal::ANIMAL_LION,
				{TArray<EAnimal>{
					EAnimal::ANIMAL_HORSE,
					EAnimal::ANIMAL_SEAL,
					EAnimal::ANIMAL_LION}},
				{TArray<EAnimal>{
					EAnimal::ANIMAL_LION,
					EAnimal::ANIMAL_SABER_TOOTH_TIGER,
					EAnimal::ANIMAL_SEAL,
					EAnimal::ANIMAL_CHAMELEON,
					EAnimal::ANIMAL_FOX}});
			break;
		case EPlayerJob::JOB_TERRORIST:
			score = CalScoreByJobs(
				animals,
				EAnimal::ANIMAL_SKUNK,
				{TArray<EAnimal>{
					EAnimal::ANIMAL_SQUID,
					EAnimal::ANIMAL_SKUNK,
					EAnimal::ANIMAL_OCTOPUS}},
				{TArray<EAnimal>{
					EAnimal::ANIMAL_CRAB,
					EAnimal::ANIMAL_LOBSTER,
					EAnimal::ANIMAL_STINGRAY,
					EAnimal::ANIMAL_EEL,
					EAnimal::ANIMAL_SKUNK}});
			break;
		case EPlayerJob::JOB_ENVIRONMENTALIST:
			score = CalScoreByJobs(
				animals,
				EAnimal::ANIMAL_PENGUIN,
				{TArray<EAnimal>{
					EAnimal::ANIMAL_PENGUIN,
					EAnimal::ANIMAL_SEAL,
					EAnimal::ANIMAL_STAR_FISH}},
				{TArray<EAnimal>{
					EAnimal::ANIMAL_PENGUIN,
					EAnimal::ANIMAL_EEL,
					EAnimal::ANIMAL_DOLPHIN,
					EAnimal::ANIMAL_WHALE,
					EAnimal::ANIMAL_SEAL}});
			break;
		default:
			break;
	}
	
	return score;
}

FScoreData UCalculateScoreByJobs::CalScoreByJobs(
	const TArray<EAnimal>& animals,
	EAnimal favoriteAnimal,
	const TArray<EAnimal>& addCombine,
	const TArray<EAnimal>& mulCombine
	)
{
	FScoreData score = {0, 0, 1};
	score.baseScore = CalBaseScore(animals, favoriteAnimal);
	if (AnimalCombinationCheck(animals, addCombine))
		score.addMulScore = 1;
	if (AnimalCombinationCheck(animals, mulCombine))
		score.mulMulScore = 3;
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

int UCalculateScoreByJobs::CalBaseScore (const TArray<EAnimal>& animals, const EAnimal& favoriteAnimal)
{
	int score = 0;
	for (const EAnimal& animal: animals)
	{
		if (animal == favoriteAnimal)
			score += 150;
		else
			score += 100;
	}
	return score;
}