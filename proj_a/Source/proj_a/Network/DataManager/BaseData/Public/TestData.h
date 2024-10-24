#pragma once

#include "CoreMinimal.h"
#include "BaseData.h"
#include "TestData.generated.h"

UCLASS()
class UTestData : public UBaseData
{
	GENERATED_BODY()
public:
	UTestData(): TestValue(0)
	{
	}

	int TestValue;
};
