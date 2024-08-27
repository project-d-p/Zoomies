#pragma once

#include "BaseMonsterCharacter.h"
#include "ChamelionCharacter.generated.h"

UCLASS()
class PROJ_A_API AChamelionCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()
public:
	AChamelionCharacter();

protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_CHAMELEON;
};
