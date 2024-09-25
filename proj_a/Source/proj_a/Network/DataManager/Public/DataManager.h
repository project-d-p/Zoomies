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

UCLASS(Blueprintable)
class UDataManager : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "DataManager")
	void AddData(const FString& Key, UBaseData* Data);
	
	UFUNCTION(BlueprintCallable, Category = "DataManager")
	UBaseData* GetData(const FString& Key);

	template <typename T>
	T* GetDataAs(const FString& Key)
	{
		UBaseData* Data = GetData(Key);
		return Data ? Cast<T>(Data) : nullptr;
	}
	
private:
	UPROPERTY()
	TMap<FString, UBaseData*> DataMap;
};
