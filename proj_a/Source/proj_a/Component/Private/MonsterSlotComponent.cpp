#include "MonsterSlotComponent.h"

#include "DPCharacter.h"
#include "FNetLogger.h"
#include "PathManager.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"

UMonsterSlotComponent::UMonsterSlotComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder
	(TEXT("/Game/material/physicalMatarial/BasicOpaqueMat"));
	if (MaterialFinder.Succeeded())
	{
		opacityMaterial = MaterialFinder.Object;
	}

	struct FAnimalInfo
	{
		FString CharacterName;
		EAnimal AnimalType;
	};

	TArray<FAnimalInfo> AnimalInfos = {
		{"CrabCharacter", EAnimal::ANIMAL_CRAB},
		{"DolphinCharacter", EAnimal::ANIMAL_DOLPHIN},
		{"EelCharacter", EAnimal::ANIMAL_EEL},
		{"ElephantCharacter", EAnimal::ANIMAL_ELEPHANT},
		{"FoxCharacter", EAnimal::ANIMAL_FOX},
		{"GiraffeCharacter", EAnimal::ANIMAL_GIRAFFE},
		{"LionCharacter", EAnimal::ANIMAL_LION},
		{"LobsterCharacter", EAnimal::ANIMAL_LOBSTER},
		{"MammothCharacter", EAnimal::ANIMAL_MAMMOTH},
		{"OctopusCharacter", EAnimal::ANIMAL_OCTOPUS},
		{"PenguinCharacter", EAnimal::ANIMAL_PENGUIN},
		{"SaberToothTigerCharacter", EAnimal::ANIMAL_SABER_TOOTH_TIGER},
		{"SealCharacter", EAnimal::ANIMAL_SEAL},
		{"SkunkCharacter", EAnimal::ANIMAL_SKUNK},
		{"SlothCharacter", EAnimal::ANIMAL_SLOTH},
		{"SquidCharacter", EAnimal::ANIMAL_SQUID},
		{"StarFishCharacter", EAnimal::ANIMAL_STAR_FISH},
		{"StingRayCharacter", EAnimal::ANIMAL_STINGRAY},
		{"WhaleCharacter", EAnimal::ANIMAL_WHALE},
		{"ChamelionCharacter", EAnimal::ANIMAL_CHAMELEON},
		{"HorseCharacter", EAnimal::ANIMAL_HORSE}
	};

	for (const auto& AnimalInfo : AnimalInfos)
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshFinder
		(PathManager::GetMonsterTransparencyMeshPath(AnimalInfo.AnimalType));
        
		if (SkeletalMeshFinder.Succeeded())
		{
			monsterMeshMap[AnimalInfo.CharacterName] = SkeletalMeshFinder.Object;
			monsterTypeMap[AnimalInfo.CharacterName] = AnimalInfo.AnimalType;
		}
	}

	monsterMeshMap["NONE"] = nullptr;
}

bool UMonsterSlotComponent::AddMonsterToSlot(ADPCharacter* character, USkeletalMesh* monster, const FString& monster_type)
{
	if (monstersInSlot.size() >= 5)
	{
		return false;
	}
	// monstersInSlot.push_back(monster);

	USkeletalMeshComponent* NewMeshComponent = NewObject<USkeletalMeshComponent>(character);
	if (!NewMeshComponent)
	{
		return false;
	}
	
	// copy the mesh and materials from the monster
	NewMeshComponent->SetSkeletalMesh(monster);

	// Set the scale to make the mesh approximately 20.0f units per side
	NewMeshComponent->RegisterComponent();
	FVector OriginalBoundsExtent = NewMeshComponent->Bounds.BoxExtent;
	float MaxExtent = FMath::Max3(OriginalBoundsExtent.X, OriginalBoundsExtent.Y, OriginalBoundsExtent.Z);
	if (MaxExtent > 0)
	{
		float ScaleFactor = 40.0f / MaxExtent;
		NewMeshComponent->SetWorldScale3D(FVector(ScaleFactor, ScaleFactor, ScaleFactor));
	}

	NewMeshComponent->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, slotName[monstersInSlot.size()]);
	
	// Set Rotation
	NewMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	
	monstersInSlot.push_back(std::make_pair(monster_type, NewMeshComponent));
	return true;
}

bool UMonsterSlotComponent::AddMonsterToSlot(ADPCharacter* character, const FString& monster_type)
{
	if (!monsterMeshMap.contains(monster_type))
	{
		return false;
	}
	return AddMonsterToSlot(character, monsterMeshMap[monster_type], monster_type);
}

bool UMonsterSlotComponent::AddMonsterToSlot(ADPCharacter* character, EAnimal monster_type)
{
	FString monsterTypeString = "";
	for (const auto& monster : monsterTypeMap)
	{
		if (monster.second == monster_type)
		{
			monsterTypeString = monster.first;
			break;
		}
	}
	if (monsterTypeString.IsEmpty())
	{
		return false;
	}
	return AddMonsterToSlot(character, monsterTypeString);
}

TArray<EAnimal> UMonsterSlotComponent::RemoveMonstersFromSlot()
{
	TArray<EAnimal> result;
	for (auto monster : monstersInSlot)
	{
		result.Add(monsterTypeMap[monster.first]);
		monster.second->DestroyComponent();
	}
	monstersInSlot.clear();
	return result;
}
