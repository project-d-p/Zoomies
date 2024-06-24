#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterAIController.h"
#include "MonsterFactory.generated.h"

UCLASS()
class UMonsterFactory : public UObject
{
	GENERATED_BODY()

public:
	ABaseMonsterAIController* RandomMonsterSpawn(const FVector& Location);
private:
	ABaseMonsterAIController* SpawnMonster(UClass* MonsterClass, const FVector& Location);
};