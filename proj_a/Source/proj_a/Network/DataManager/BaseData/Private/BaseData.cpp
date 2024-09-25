#include "BaseData.h"
#include "UObject/ConstructorHelpers.h"

UBaseData::UBaseData()
	: DataID(TEXT("")), DataVersion(1), bIsDirty(false)
{
}

void UBaseData::InitializeData_Implementation()
{
	DataID = FGuid::NewGuid().ToString();
	DataVersion = 1;
	bIsDirty = false;

	OnDataChanged.Broadcast();
}

bool UBaseData::ValidateData_Implementation()
{
	return !DataID.IsEmpty();
}

void UBaseData::ResetData_Implementation()
{
	DataID = TEXT("");
	DataVersion = 1;
	bIsDirty = false;

	OnDataChanged.Broadcast();
}

void UBaseData::SaveData_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SaveData called on UBaseData. Override this method in a derived class."))
}

void UBaseData::LoadData_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("LoadData called on UBaseData. Override this method in a derived class."))
}

UBaseData* UBaseData::Clone_Implementation()
{
	UBaseData* NewData = NewObject<UBaseData>(GetOuter(), GetClass());
	NewData->DataID = DataID;
	NewData->DataVersion = DataVersion;
	NewData->bIsDirty = bIsDirty;

	// Clone any additional data members on derived classes
	return NewData;
}

FName UBaseData::GetDataType() const
{
	return GetClass()->GetFName();
}

void UBaseData::MarkDirty()
{
	bIsDirty = true;
	OnDataChanged.Broadcast();
}