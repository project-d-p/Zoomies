#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterCharacter.h"
#include "DPCharacter.h"
#include "Components/ActorComponent.h"
#include <map>

#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "MonsterSlotComponent.generated.h"

UCLASS()
class UMonsterSlotComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMonsterSlotComponent();
	bool AddMonsterToSlot(ADPCharacter* character, USkeletalMesh* monster, const FString& monster_type);
	bool AddMonsterToSlot(ADPCharacter* character, const FString& monster_type);
	bool AddMonsterToSlot(ADPCharacter* character, EAnimal monster_type);
	TArray<EAnimal> RemoveMonstersFromSlot();

private:
	UPROPERTY(EditAnywhere, Category = "Setup")
	UMaterialInterface* opacityMaterial;
	
	FName slotName[5] = { "MonsterSlot_0", "MonsterSlot_1", "MonsterSlot_2", "MonsterSlot_3", "MonsterSlot_4" };
	std::vector<std::pair<FString, USkeletalMeshComponent*>> monstersInSlot;

	std::map<FString, USkeletalMesh*> monsterMeshMap;
	std::map<FString, EAnimal> monsterTypeMap;
};
