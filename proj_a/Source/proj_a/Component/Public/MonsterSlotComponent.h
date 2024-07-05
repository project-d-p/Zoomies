#pragma once

#include "CoreMinimal.h"
#include "BaseMonsterCharacter.h"
#include "Components/ActorComponent.h"
#include <map>
#include "MonsterSlotComponent.generated.h"

UCLASS()
class UMonsterSlotComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMonsterSlotComponent();
	bool AddMonsterToSlot(ADPCharacter* character, USkeletalMesh* monster);
	bool AddMonsterToSlot(ADPCharacter* character, const FString& monster_type);
	void RemoveMonstersFromSlot();

private:
	UPROPERTY(EditAnywhere, Category = "Setup")
	UMaterialInterface* opacityMaterial;
	
	FName slotName[5] = { "MonsterSlot_0", "MonsterSlot_1", "MonsterSlot_2", "MonsterSlot_3", "MonsterSlot_4" };
	std::vector<USkeletalMeshComponent*> monstersInSlot;

	std::map<FString, USkeletalMesh*> monsterMeshMap;
};
