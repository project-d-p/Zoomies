#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterAIController.h"
#include "MonsterFactory.generated.h"

UCLASS()
class UMonsterFactory : public UObject
{
	GENERATED_BODY()

public:
	ABaseMonsterAIController* RandomMonsterSpawn(int32 idx);
private:
	ABaseMonsterAIController* SpawnMonster(UClass* MonsterClass, const FVector& Location, int32 idx);
};