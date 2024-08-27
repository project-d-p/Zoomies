#pragma once

#include "BaseMonsterCharacter.h"
#include "SlothCharacter.generated.h"

UCLASS()
class PROJ_A_API ASlothCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	ASlothCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_SLOTH;
};