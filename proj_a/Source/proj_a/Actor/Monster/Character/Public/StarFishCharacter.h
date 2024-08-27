#pragma once

#include "BaseMonsterCharacter.h"
#include "StarFishCharacter.generated.h"

UCLASS()
class PROJ_A_API AStarFishCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	AStarFishCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_STAR_FISH;
};