#include "TimeData.h"

void UTimeData::InitializeData_Implementation()
{
	Super::InitializeData_Implementation();
	TimeRemaining = 0.0f;
}

bool UTimeData::ValidateData_Implementation()
{
	bool bIsValid = Super::ValidateData_Implementation();
	bIsValid &= TimeRemaining >= 0.0f;
	return bIsValid;
}

void UTimeData::ResetData_Implementation()
{
	Super::ResetData_Implementation();
	TimeRemaining = 0.0f;
}

UBaseData* UTimeData::Clone_Implementation(UObject* Outer)
{
	UTimeData* ClonedObject = Cast<UTimeData>(Super::Clone_Implementation(Outer));
	if (ClonedObject)
	{
		ClonedObject->TimeRemaining = TimeRemaining;
	}
	return ClonedObject;
}

FName UTimeData::GetDataType() const
{
	return Super::GetDataType();
}

void UTimeData::SetTimeRemaining(float NewTime)
{
	TimeRemaining = NewTime;
}
