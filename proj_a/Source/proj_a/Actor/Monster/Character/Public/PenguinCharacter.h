#pragma once

#include "BaseMonsterCharacter.h"
#include "PenguinCharacter.generated.h"

UCLASS()
class PROJ_A_API APenguinCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	APenguinCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_PENGUIN;
};