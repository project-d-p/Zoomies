#pragma once

#include "BaseMonsterCharacter.h"
#include "FoxCharacter.generated.h"

UCLASS()
class PROJ_A_API AFoxCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	AFoxCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_FOX;
};
