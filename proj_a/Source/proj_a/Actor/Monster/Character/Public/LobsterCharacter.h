#pragma once

#include "BaseMonsterCharacter.h"
#include "LobsterCharacter.generated.h"

UCLASS()
class PROJ_A_API ALobsterCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	ALobsterCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_LOBSTER;
};
