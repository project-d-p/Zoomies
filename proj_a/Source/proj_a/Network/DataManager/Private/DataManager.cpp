#include "DataManager.h"

#include "FNetLogger.h"

void UDataManager::AddDataToArray(const FString& Key, UBaseData* Data)
{
	if (Data && Data->ValidateData())
	{
		if (UDataArray** ExistingDataArray = DataArrayMap.Find(Key))
		{
			(*ExistingDataArray)->AddData(Data);
		}
		else
		{
			UDataArray* NewDataArray = NewObject<UDataArray>(this);
			NewDataArray->AddData(Data);

			DataArrayMap.Add(Key, NewDataArray);
		}
	}
}



void UDataManager::AddDataToSingle(const FString& Key, UBaseData* Data)
{
	if (Data && Data->ValidateData())
	{
		DataSingleMap.Add(Key, Data);
	}
}

UBaseData* UDataManager::GetData(const FString& Key)
{
	UBaseData** DataPtr = DataSingleMap.Find(Key);
	return DataPtr ? *DataPtr : nullptr;
}

UDataArray* UDataManager::GetDataArray(const FString& Key)
{
	UDataArray** DataArrayPtr = DataArrayMap.Find(Key);
	return DataArrayPtr ? *DataArrayPtr : nullptr;
}

UDataArray* UDataManager::GetSeamlessDataArray(const FString& Key)
{
	UDataArray** DataArrayPtr = SeamlessDataArrayMap.Find(Key);
	return DataArrayPtr ? *DataArrayPtr : nullptr;
}

void UDataManager::ClearSeamlessDataArray()
{
	SeamlessDataArrayMap.Empty();
}

void UDataManager::AddSeamlessDataToArray(const FString& Key, UBaseData* Data)
{
	if (Data && Data->ValidateData())
	{
		if (UDataArray** ExistingDataArray = SeamlessDataArrayMap.Find(Key))
		{
			(*ExistingDataArray)->AddData(Data);
		}
		else
		{
			UDataArray* NewDataArray = NewObject<UDataArray>(this);
			NewDataArray->AddData(Data);

			SeamlessDataArrayMap.Add(Key, NewDataArray);
		}
	}
}
