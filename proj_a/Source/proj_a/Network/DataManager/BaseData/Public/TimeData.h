#pragma once

#include "CoreMinimal.h"
#include "BaseData.h"
#include "TimeData.generated.h"

UCLASS()
class UTimeData : public UBaseData
{
	GENERATED_BODY()
public:
	virtual void InitializeData_Implementation() override;
	virtual bool ValidateData_Implementation() override;
	virtual void ResetData_Implementation() override;
	virtual UBaseData* Clone_Implementation(UObject* Outer) override;
	virtual FName GetDataType() const override;
	void SetTimeRemaining(float NewTime);
	
private:
	float TimeRemaining;
};
