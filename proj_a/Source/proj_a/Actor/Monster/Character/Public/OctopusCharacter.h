#pragma once

#include "BaseMonsterCharacter.h"
#include "OctopusCharacter.generated.h"

UCLASS()
class PROJ_A_API AOctopusCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	AOctopusCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_OCTOPUS;
};