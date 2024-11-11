#pragma once

#include "CoreMinimal.h"
#include "BaseData.h"
#include "DataManager.generated.h"

// Client ȣ��Ǵ� �۾�
	// PlayerController ȣ�� �ȵ� - �ٸ� Ŭ���̾�Ʈ
	// GM ȣ�� �ȵ�

	// Actor -> PlayerState�� ��� ����
	// Host�� ������� ����. - `Actor->GetPlayerState()->GetPlayerName()`

	// PlayerState - �������� ���� - ex) ���� - BeginPlay() - Load()
	// Character(Actor) - ���� ������ ���� - ex) ���� ���� - BeginPlay() - Load()
	//	�� Monster - Save() (Save�ϴ� ��ü�� �����Ѵ�)
	//  �� Player - Load() Ŭ���̾�Ʈ�� �����ϴ� ��ü
	//  �� NPC
	//  �� ETC
	// GameState - ��ü �帧 ���� - ex) �ð� - BeginPlay() - Load()
	// GameMode(Load �۾��� �ϴ� ��ü) - BeginPlay() - Load()
	//	�� Monster - Load() - �������� �����Ǵ� ��ü�� GameMode���� Load()�� ȣ���Ѵ�.
	//  �� NPC - Load()
//

// ����. ��� Actor�� ���� �ڵ����� ������Ű�� �ȴٸ�? ��� �ɱ�?
//   �� ����Ǵ� ���� Controller�� ��� �������� ���ΰ�?

// ��� ���ʹ� �� ������ ������ �ȴ�.
// 1. Controller�� �����ϴ� ��ü ex) �÷��̾�, ����, NPC
// 2. Controller�� �������� �ʴ� ��ü ex) �÷��̾ ��ġ�� ���� ��ü (��, Ʈ���� �� ��)

// 2���� ��� ������ �� �����Ǵ� �������� �ڵ����� �������ѵ� �ȴ�.
// 1���� ���, GameMode �� Level�� �� �����Ǵ� �������� �ڵ����� ������Ű�� �ȵȴ�.
//  �� ������ �ڵ����� �����ϴ� ������ �ֱ� �����̴�.
//  �� �׷� 1���� ��� ��� �����͸� Load�� ���ΰ�?

// ���� �����ϴ� �� Ŭ������ ������ �����ΰ�?
// �ܼ��� �����͸� �����ϰ� �ε��� �� �ְԲ� �ϴ� ���� �����̴�.
// �׷� �� �����͸� ��� ����� �������� ����ϴ� ���� ������� �ƴѰ�..?
// �׷��ٸ� ���� ��� �����͸� �����ϰ� �ҷ��� �� �ִ����� ���ؼ� ����ϸ� �ȴ�.

UCLASS()
class UDataArray : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UBaseData*> DataArray;

	// �迭�� ������ �߰�
	void AddData(UBaseData* Data)
	{
		if (Data && Data->ValidateData())
		{
			DataArray.Add(Data);
		}
	}
};

UCLASS(Blueprintable)
class UDataManager : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "DataManager")
	void AddDataToArray(const FString& Key, UBaseData* Data);

	UFUNCTION(BlueprintCallable, Category = "DataManager")
	void AddDataToSingle(const FString& Key, UBaseData* Data);
	
	UFUNCTION(BlueprintCallable, Category = "DataManager")
	UBaseData* GetData(const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "DataManager")
	UDataArray* GetDataArray(const FString& Key);

	template <typename T>
	T* GetSingleDataAs(const FString& Key)
	{
		UBaseData* Data = GetData(Key);
		return Data ? Cast<T>(Data) : nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "DataManager")
	bool IsEmpty() const
	{
		return DataArrayMap.Num() == 0 && DataSingleMap.Num() == 0;
	}

	void Clear()
	{
		TArray<FString> KeysToRemove; // ������ Key�� ������ �迭
		// DataArrayMap.Empty();
		for (auto& Elem : DataArrayMap)
		{
			if (Elem.Key != "PlayerScoreSeamless") // Key�� PlayerScoreSeamless�� �ƴ� ���
			{
				KeysToRemove.Add(Elem.Key); // ���� ��� Key�� ����
			}
		}

		// ������ Key�� �̿��� TMap���� ����
		for (const FString& Key : KeysToRemove)
		{
			DataArrayMap.Remove(Key); // Key�� �ش��ϴ� ��Ҹ� ����
		}
		DataSingleMap.Empty();
	}

	void RemoveData(const FString& Key)
	{
		DataSingleMap.Remove(Key);
	}

	void RemoveDataArray(const FString& Key)
	{
		DataArrayMap.Remove(Key);
	}

private:
	UPROPERTY()
	TMap<FString, UDataArray*> DataArrayMap;
	UPROPERTY()
	TMap<FString, UBaseData*> DataSingleMap;
};
