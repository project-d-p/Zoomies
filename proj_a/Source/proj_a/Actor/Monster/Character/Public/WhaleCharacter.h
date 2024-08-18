#pragma once

#include "BaseMonsterCharacter.h"
#include "WhaleCharacter.generated.h"

UCLASS()
class PROJ_A_API AWhaleCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	AWhaleCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_WHALE;
};