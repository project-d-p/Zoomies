#include "DataManager.h"

#include "FNetLogger.h"

void UDataManager::AddData(const FString& Key, UBaseData* Data)
{
	if (Data && Data->ValidateData())
	{
		DataMap.Add(Key, Data);
	}
}

UBaseData* UDataManager::GetData(const FString& Key)
{
	UBaseData** DataPtr = DataMap.Find(Key);
	return DataPtr ? *DataPtr : nullptr;
}
