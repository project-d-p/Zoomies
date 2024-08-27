#pragma once

#include "BaseMonsterCharacter.h"
#include "SealCharacter.generated.h"

UCLASS()
class PROJ_A_API ASealCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	ASealCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_SEAL;
};