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
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CRAB
	(PathManager::GetMonsterPath(EMonster::CRAB));
	if (SK_CRAB.Succeeded()) {
		monsterMeshMap["CrabCharacter"] = SK_CRAB.Object;
		monsterTypeMap["CrabCharacter"] = EAnimal::ANIMAL_CRAB;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_DOLPHIN
	(PathManager::GetMonsterPath(EMonster::DOLPHIN));
	if (SK_DOLPHIN.Succeeded()) {
		monsterMeshMap["DolphinCharacter"] = SK_DOLPHIN.Object;
		monsterTypeMap["DolphinCharacter"] = EAnimal::ANIMAL_DOLPHIN;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_EEL
	(PathManager::GetMonsterPath(EMonster::EEL));
	if (SK_EEL.Succeeded()) {
		monsterMeshMap["EelCharacter"] = SK_EEL.Object;
		monsterTypeMap["EelCharacter"] = EAnimal::ANIMAL_EEL;
	}
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ELEPHANT
    (PathManager::GetMonsterPath(EMonster::ELEPHANT));
    if (SK_ELEPHANT.Succeeded()) {
		monsterMeshMap["ElephantCharacter"] = SK_ELEPHANT.Object;
    	monsterTypeMap["ElephantCharacter"] = EAnimal::ANIMAL_ELEPHANT;
    }

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOX
	(PathManager::GetMonsterPath(EMonster::FOX));
	if (SK_FOX.Succeeded()) {
		monsterMeshMap["FoxCharacter"] = SK_FOX.Object;
		monsterTypeMap["FoxCharacter"] = EAnimal::ANIMAL_FOX;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_GIRAFFE
	(PathManager::GetMonsterPath(EMonster::GIRAFFE));
	if (SK_GIRAFFE.Succeeded()) {
		monsterMeshMap["GiraffeCharacter"] = SK_GIRAFFE.Object;
		monsterTypeMap["GiraffeCharacter"] = EAnimal::ANIMAL_GIRAFFE;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LION
	(PathManager::GetMonsterPath(EMonster::LION));
	if (SK_LION.Succeeded()) {
		monsterMeshMap["LionCharacter"] = SK_LION.Object;
		monsterTypeMap["LionCharacter"] = EAnimal::ANIMAL_LION;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LOBSTER
	(PathManager::GetMonsterPath(EMonster::LOBSTER));
	if (SK_LOBSTER.Succeeded()) {
		monsterMeshMap["LobsterCharacter"] = SK_LOBSTER.Object;
		monsterTypeMap["LobsterCharacter"] = EAnimal::ANIMAL_LOBSTER;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH
	(PathManager::GetMonsterPath(EMonster::MAMMOTH));
	if (SK_MAMMOTH.Succeeded()) {
		monsterMeshMap["MammothCharacter"] = SK_MAMMOTH.Object;
		monsterTypeMap["MammothCharacter"] = EAnimal::ANIMAL_MAMMOTH;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_OCTOPUS
	(PathManager::GetMonsterPath(EMonster::OCTOPUS));
	if (SK_OCTOPUS.Succeeded()) {
		monsterMeshMap["OctopusCharacter"] = SK_OCTOPUS.Object;
		monsterTypeMap["OctopusCharacter"] = EAnimal::ANIMAL_OCTOPUS;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PENGUIN
	(PathManager::GetMonsterPath(EMonster::PENGUIN));
	if (SK_PENGUIN.Succeeded()) {
		monsterMeshMap["PenguinCharacter"] = SK_PENGUIN.Object;
		monsterTypeMap["PenguinCharacter"] = EAnimal::ANIMAL_PENGUIN;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_RABBIT
	(PathManager::GetMonsterPath(EMonster::RABBIT));
	if (SK_RABBIT.Succeeded()) {
		monsterMeshMap["RabbitCharacter"] = SK_RABBIT.Object;
		monsterTypeMap["RabbitCharacter"] = EAnimal::ANIMAL_RABBIT;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SABERTOOTHTIGER
	(PathManager::GetMonsterPath(EMonster::SABER_TOOTH_TIGER));
	if (SK_SABERTOOTHTIGER.Succeeded()) {
		monsterMeshMap["SaberToothTigerCharacter"] = SK_SABERTOOTHTIGER.Object;
		monsterTypeMap["SaberToothTigerCharacter"] = EAnimal::ANIMAL_SABER_TOOTH_TIGER;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SEAL
	(PathManager::GetMonsterPath(EMonster::SEAL));
	if (SK_SEAL.Succeeded()) {
		monsterMeshMap["SealCharacter"] = SK_SEAL.Object;
		monsterTypeMap["SealCharacter"] = EAnimal::ANIMAL_SEAL;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SHEPHERD
	(PathManager::GetMonsterPath(EMonster::SHEPHERD));
	if (SK_SHEPHERD.Succeeded()) {
		monsterMeshMap["ShepherdCharacter"] = SK_SHEPHERD.Object;
		monsterTypeMap["ShepherdCharacter"] = EAnimal::ANIMAL_SHEPHERD;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SKUNK
	(PathManager::GetMonsterPath(EMonster::SKUNK));
	if (SK_SKUNK.Succeeded()) {
		monsterMeshMap["SkunkCharacter"] = SK_SKUNK.Object;
		monsterTypeMap["SkunkCharacter"] = EAnimal::ANIMAL_SKUNK;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SLOTH
	(PathManager::GetMonsterPath(EMonster::SLOTH));
	if (SK_SLOTH.Succeeded()) {
		monsterMeshMap["SlothCharacter"] = SK_SLOTH.Object;
		monsterTypeMap["SlothCharacter"] = EAnimal::ANIMAL_SLOTH;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SQUID
	(PathManager::GetMonsterPath(EMonster::SQUID));
	if (SK_SQUID.Succeeded()) {
		monsterMeshMap["SquidCharacter"] = SK_SQUID.Object;
		monsterTypeMap["SquidCharacter"] = EAnimal::ANIMAL_SQUID;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STARFISH
	(PathManager::GetMonsterPath(EMonster::STAR_FISH));
	if (SK_STARFISH.Succeeded()) {
		monsterMeshMap["StarFishCharacter"] = SK_STARFISH.Object;
		monsterTypeMap["StarFishCharacter"] = EAnimal::ANIMAL_STAR_FISH;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STINGRAY
	(PathManager::GetMonsterPath(EMonster::STINGRAY));
	if (SK_STINGRAY.Succeeded()) {
		monsterMeshMap["StingRayCharacter"] = SK_STINGRAY.Object;
		monsterTypeMap["StingRayCharacter"] = EAnimal::ANIMAL_STINGRAY;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WHALE
	(PathManager::GetMonsterPath(EMonster::WHALE));
	if (SK_WHALE.Succeeded()) {
		monsterMeshMap["WhaleCharacter"] = SK_WHALE.Object;
		monsterTypeMap["WhaleCharacter"] = EAnimal::ANIMAL_WHALE;
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
