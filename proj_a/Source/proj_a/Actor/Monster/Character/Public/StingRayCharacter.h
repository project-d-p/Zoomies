#pragma once

#include "BaseMonsterCharacter.h"
#include "StingRayCharacter.generated.h"

UCLASS()
class PROJ_A_API AStingRayCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	AStingRayCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_STINGRAY;
};