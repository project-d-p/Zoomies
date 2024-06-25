#pragma once

#include "CoreMinimal.h"
#include "MonsterFactory.generated.h"

UCLASS()
class UMonsterFactory : public UObject
{
	GENERATED_BODY()

public:
	void SpawnMonster(UClass* MonsterClass, const FVector& Location);
};