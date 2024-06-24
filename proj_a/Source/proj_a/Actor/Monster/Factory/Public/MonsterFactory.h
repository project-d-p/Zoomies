#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterAIController.h"
#include "MonsterFactory.generated.h"

UCLASS()
class UMonsterFactory : public UObject
{
	GENERATED_BODY()

public:
	ABaseMonsterAIController* SpawnMonster(UClass* MonsterClass, const FVector& Location);
};