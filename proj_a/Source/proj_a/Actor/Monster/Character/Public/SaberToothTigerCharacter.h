#pragma once

#include "BaseMonsterCharacter.h"
#include "SaberToothTigerCharacter.generated.h"

UCLASS()
class PROJ_A_API ASaberToothTigerCharacter : public ABaseMonsterCharacter
{
	GENERATED_BODY()

public:
	ASaberToothTigerCharacter();
protected:
	const EAnimal AnimalType = EAnimal::ANIMAL_SABER_TOOTH_TIGER;
};