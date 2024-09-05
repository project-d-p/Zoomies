#pragma once

#include "BaseMonsterCharacter.h"
#include "LionCharacter.generated.h"

UCLASS()
class PROJ_A_API ALionCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	ALionCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_LION;
};