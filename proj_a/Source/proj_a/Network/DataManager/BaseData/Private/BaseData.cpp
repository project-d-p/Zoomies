#include "BaseData.h"
#include "UObject/ConstructorHelpers.h"

void UBaseData::InitializeData_Implementation()
{
	DataID = FGuid::NewGuid().ToString();
	DataVersion = 1;
	
	OnDataChanged.Broadcast(this);
}

bool UBaseData::ValidateData_Implementation()
{
	return !DataID.IsEmpty();
}

void UBaseData::ResetData_Implementation()
{
	DataID = TEXT("");
	DataVersion = 1;

	OnDataChanged.Broadcast(this);
}

void UBaseData::SaveData_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SaveData called on UBaseData. Override this method in a derived class."))
}

void UBaseData::LoadData_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("LoadData called on UBaseData. Override this method in a derived class."))
}

UBaseData* UBaseData::Clone_Implementation(UObject* Outer)
{
	UBaseData* NewData = NewObject<UBaseData>(Outer, GetClass());
	NewData->DataID = DataID;
	NewData->DataVersion = DataVersion;

	// Clone any additional data members on derived classes
	return NewData;
}

FName UBaseData::GetDataType() const
{
	return GetClass()->GetFName();
}

void UBaseData::OnDataChangedBroadcast()
{
	OnDataChanged.Broadcast(this);
}

void UBaseData::CloneData(UBaseData* NewData)
{
	NewData->DataID = DataID;
	NewData->DataVersion = DataVersion;
}
