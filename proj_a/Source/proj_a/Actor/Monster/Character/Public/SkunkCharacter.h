#pragma once

#include "BaseMonsterCharacter.h"
#include "SkunkCharacter.generated.h"

UCLASS()
class PROJ_A_API ASkunkCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	ASkunkCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_SKUNK;
};