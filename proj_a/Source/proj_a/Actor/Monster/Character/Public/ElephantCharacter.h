#pragma once

#include "BaseMonsterCharacter.h"
#include "ElephantCharacter.generated.h"

UCLASS()
class PROJ_A_API AElephantCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	AElephantCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_ELEPHANT;
};
