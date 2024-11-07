#include "JudgeData.h"

void UJudgeData::InitializeData_Implementation()
{
	Super::InitializeData_Implementation();

	JudgedPlayerNames.Empty();
}

bool UJudgeData::ValidateData_Implementation()
{
	return Super::ValidateData_Implementation();
}

void UJudgeData::ResetData_Implementation()
{
	Super::ResetData_Implementation();

	JudgedPlayerNames.Empty();
}

UBaseData* UJudgeData::Clone_Implementation(UObject* Outer)
{
	UJudgeData* ClonedObject = Cast<UJudgeData>(Super::Clone_Implementation(Outer));
	if (ClonedObject)
	{
		ClonedObject->JudgedPlayerNames = JudgedPlayerNames;
	}
	return ClonedObject;
}

FName UJudgeData::GetDataType() const
{
	return Super::GetDataType();
}

void UJudgeData::AddJudgedPlayerName(const FString& NewName)
{
	if (!IsJudgedPlayer(NewName))
		JudgedPlayerNames.Add(NewName);
}

bool UJudgeData::IsJudgedPlayer(const FString& Name) const
{
	for (const FString& JudgedName : JudgedPlayerNames)
	{
		if (JudgedName == Name)
			return true;
	}
	return false;
}
