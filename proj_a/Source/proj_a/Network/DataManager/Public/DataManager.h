#pragma once

#include "CoreMinimal.h"
#include "BaseData.h"
#include "DataManager.generated.h"

// Client 호출되는 작업
	// PlayerController 호출 안됨 - 다른 클라이언트
	// GM 호출 안됨

	// Actor -> PlayerState를 들고 있음
	// Host가 만들어준 거임. - `Actor->GetPlayerState()->GetPlayerName()`

	// PlayerState - 개인적인 정보 - ex) 점수 - BeginPlay() - Load()
	// Character(Actor) - 액터 내부의 정보 - ex) 무기 정보 - BeginPlay() - Load()
	//	ㄴ Monster - Save() (Save하는 객체로 인정한다)
	//  ㄴ Player - Load() 클라이언트가 관리하는 객체
	//  ㄴ NPC
	//  ㄴ ETC
	// GameState - 전체 흐름 정보 - ex) 시간 - BeginPlay() - Load()
	// GameMode(Load 작업만 하는 객체) - BeginPlay() - Load()
	//	ㄴ Monster - Load() - 서버에서 관리되는 객체는 GameMode에서 Load()를 호출한다.
	//  ㄴ NPC - Load()
//

// 가정. 모든 Actor를 내가 자동으로 생성시키게 된다면? 어떻게 될까?
//   ㄴ 제기되는 문제 Controller는 어떻게 소지해줄 것인가?

// 모든 액터는 두 가지로 나뉘게 된다.
// 1. Controller를 소지하는 객체 ex) 플레이어, 몬스터, NPC
// 2. Controller를 소지하지 않는 객체 ex) 플레이어가 설치한 정적 객체 (벽, 트리거 존 등)

// 2번의 경우 세션이 재 생성되는 과정에서 자동으로 생성시켜도 된다.
// 1번의 경우, GameMode 및 Level이 재 생성되는 과정에서 자동으로 생성시키면 안된다.
//  ㄴ 레벨이 자동으로 생성하는 로직이 있기 때문이다.
//  ㄴ 그럼 1번의 경우 어떻게 데이터를 Load할 것인가?

// 내가 설계하는 이 클래스의 목적은 무엇인가?
// 단순히 데이터를 저장하고 로드할 수 있게끔 하는 것이 목적이다.
// 그럼 이 데이터를 어떻게 사용할 것인지는 사용하는 자의 마음대로 아닌가..?
// 그렇다면 나는 어떻게 데이터를 저장하고 불러올 수 있는지에 대해서 고민하면 된다.

UCLASS()
class UDataArray : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UBaseData*> DataArray;

	// 배열에 데이터 추가
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
		TArray<FString> KeysToRemove; // 삭제할 Key를 저장할 배열
		// DataArrayMap.Empty();
		for (auto& Elem : DataArrayMap)
		{
			if (Elem.Key != "PlayerScoreSeamless") // Key가 PlayerScoreSeamless가 아닌 경우
			{
				KeysToRemove.Add(Elem.Key); // 삭제 대상 Key를 수집
			}
		}

		// 수집한 Key를 이용해 TMap에서 삭제
		for (const FString& Key : KeysToRemove)
		{
			DataArrayMap.Remove(Key); // Key에 해당하는 요소를 제거
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
