#include "MonsterSlotComponent.h"

#include "DPCharacter.h"
#include "FNetLogger.h"
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
	(TEXT("/Game/model/animals/crab/crab.crab"));
	if (SK_CRAB.Succeeded()) {
		monsterMeshMap["CrabCharacter"] = SK_CRAB.Object;
		monsterTypeMap["CrabCharacter"] = EAnimal::ANIMAL_CRAB;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_DOLPHIN
	(TEXT("/Game/model/animals/dolphin/dolphin1.dolphin1"));
	if (SK_DOLPHIN.Succeeded()) {
		monsterMeshMap["DolphinCharacter"] = SK_DOLPHIN.Object;
		monsterTypeMap["DolphinCharacter"] = EAnimal::ANIMAL_DOLPHIN;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_EEL
	(TEXT("/Game/model/animals/eel/eel.eel"));
	if (SK_EEL.Succeeded()) {
		monsterMeshMap["EelCharacter"] = SK_EEL.Object;
		monsterTypeMap["EelCharacter"] = EAnimal::ANIMAL_EEL;
	}
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ELEPHANT
    (TEXT("/Game/model/animals/elephant/low_poly_elephant.low_poly_elephant"));
    if (SK_ELEPHANT.Succeeded()) {
		monsterMeshMap["ElephantCharacter"] = SK_ELEPHANT.Object;
    	monsterTypeMap["ElephantCharacter"] = EAnimal::ANIMAL_ELEPHANT;
    }

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOX
	(TEXT("/Game/model/animals/fox_1/fennec_fox.fennec_fox"));
	if (SK_FOX.Succeeded()) {
		monsterMeshMap["FoxCharacter"] = SK_FOX.Object;
		monsterTypeMap["FoxCharacter"] = EAnimal::ANIMAL_FOX;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_GIRAFFE
	(TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly"));
	if (SK_GIRAFFE.Succeeded()) {
		monsterMeshMap["GiraffeCharacter"] = SK_GIRAFFE.Object;
		monsterTypeMap["GiraffeCharacter"] = EAnimal::ANIMAL_GIRAFFE;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LION
	(TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion"));
	if (SK_LION.Succeeded()) {
		monsterMeshMap["LionCharacter"] = SK_LION.Object;
		monsterTypeMap["LionCharacter"] = EAnimal::ANIMAL_LION;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LOBSTER
	(TEXT("/Game/model/animals/lobstar/Object_7.Object_7"));
	if (SK_LOBSTER.Succeeded()) {
		monsterMeshMap["LobsterCharacter"] = SK_LOBSTER.Object;
		monsterTypeMap["LobsterCharacter"] = EAnimal::ANIMAL_LOBSTER;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH
	(TEXT("/Game/model/animals/mammoth/sm_mammoth.sm_mammoth"));
	if (SK_MAMMOTH.Succeeded()) {
		monsterMeshMap["MammothCharacter"] = SK_MAMMOTH.Object;
		monsterTypeMap["MammothCharacter"] = EAnimal::ANIMAL_MAMMOTH;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_OCTOPUS
	(TEXT("/Game/model/animals/octopus/Octopus1.Octopus1"));
	if (SK_OCTOPUS.Succeeded()) {
		monsterMeshMap["OctopusCharacter"] = SK_OCTOPUS.Object;
		monsterTypeMap["OctopusCharacter"] = EAnimal::ANIMAL_OCTOPUS;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PENGUIN
	(TEXT("/Game/model/animals/penguin/penguin_swimming.penguin_swimming"));
	if (SK_PENGUIN.Succeeded()) {
		monsterMeshMap["PenguinCharacter"] = SK_PENGUIN.Object;
		monsterTypeMap["PenguinCharacter"] = EAnimal::ANIMAL_PENGUIN;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_RABBIT
	(TEXT("/Game/model/animals/rabbit/low_poly_rabbit.low_poly_rabbit"));
	if (SK_RABBIT.Succeeded()) {
		monsterMeshMap["RabbitCharacter"] = SK_RABBIT.Object;
		monsterTypeMap["RabbitCharacter"] = EAnimal::ANIMAL_RABBIT;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SABERTOOTHTIGER
	(TEXT("/Game/model/animals/sabertoothTiger/sm_sabertoothTiger.sm_sabertoothTiger"));
	if (SK_SABERTOOTHTIGER.Succeeded()) {
		monsterMeshMap["SaberToothTigerCharacter"] = SK_SABERTOOTHTIGER.Object;
		monsterTypeMap["SaberToothTigerCharacter"] = EAnimal::ANIMAL_SABER_TOOTH_TIGER;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SEAL
	(TEXT("/Game/model/animals/seal/seal1.seal1"));
	if (SK_SEAL.Succeeded()) {
		monsterMeshMap["SealCharacter"] = SK_SEAL.Object;
		monsterTypeMap["SealCharacter"] = EAnimal::ANIMAL_SEAL;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SHEPHERD
	(TEXT("/Game/model/animals/shepherd/stylized_low_poly_german_shepherd.stylized_low_poly_german_shepherd"));
	if (SK_SHEPHERD.Succeeded()) {
		monsterMeshMap["ShepherdCharacter"] = SK_SHEPHERD.Object;
		monsterTypeMap["ShepherdCharacter"] = EAnimal::ANIMAL_SHEPHERD;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SKUNK
	(TEXT("/Game/model/animals/skunk/low_poly_skunk.low_poly_skunk"));
	if (SK_SKUNK.Succeeded()) {
		monsterMeshMap["SkunkCharacter"] = SK_SKUNK.Object;
		monsterTypeMap["SkunkCharacter"] = EAnimal::ANIMAL_SKUNK;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SLOTH
	(TEXT("/Game/model/animals/sloth/sm_sloth.sm_sloth"));
	if (SK_SLOTH.Succeeded()) {
		monsterMeshMap["SlothCharacter"] = SK_SLOTH.Object;
		monsterTypeMap["SlothCharacter"] = EAnimal::ANIMAL_SLOTH;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SQUID
	(TEXT("/Game/model/animals/squid/Squid1.Squid1"));
	if (SK_SQUID.Succeeded()) {
		monsterMeshMap["SquidCharacter"] = SK_SQUID.Object;
		monsterTypeMap["SquidCharacter"] = EAnimal::ANIMAL_SQUID;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STARFISH
	(TEXT("/Game/model/animals/starFish/star_fish.star_fish"));
	if (SK_STARFISH.Succeeded()) {
		monsterMeshMap["StarFishCharacter"] = SK_STARFISH.Object;
		monsterTypeMap["StarFishCharacter"] = EAnimal::ANIMAL_STAR_FISH;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STINGRAY
	(TEXT("/Game/model/animals/stingRay/sting_ray.sting_ray"));
	if (SK_STINGRAY.Succeeded()) {
		monsterMeshMap["StingRayCharacter"] = SK_STINGRAY.Object;
		monsterTypeMap["StingRayCharacter"] = EAnimal::ANIMAL_STINGRAY;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WHALE
	(TEXT("/Game/model/animals/whale/whale1.whale1"));
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
