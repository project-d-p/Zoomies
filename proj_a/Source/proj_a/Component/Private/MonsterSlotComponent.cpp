#include "MonsterSlotComponent.h"

#include "FNetLogger.h"

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
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_DOLPHIN
	(TEXT("/Game/model/animals/dolphin/dolphin1.dolphin1"));
	if (SK_DOLPHIN.Succeeded()) {
		monsterMeshMap["DolphinCharacter"] = SK_DOLPHIN.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_EEL
	(TEXT("/Game/model/animals/eel/eel.eel"));
	if (SK_EEL.Succeeded()) {
		monsterMeshMap["EelCharacter"] = SK_EEL.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ELEPHANT
    (TEXT("/Game/model/animals/elephant/low_poly_elephant.low_poly_elephant"));
    if (SK_ELEPHANT.Succeeded()) {
		monsterMeshMap["ElephantCharacter"] = SK_ELEPHANT.Object;
    }

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOX
	(TEXT("/Game/model/animals/fox_1/fennec_fox.fennec_fox"));
	if (SK_FOX.Succeeded()) {
		monsterMeshMap["FoxCharacter"] = SK_FOX.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_GIRAFFE
	(TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly"));
	if (SK_GIRAFFE.Succeeded()) {
		monsterMeshMap["GiraffeCharacter"] = SK_GIRAFFE.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LION
	(TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion"));
	if (SK_LION.Succeeded()) {
		monsterMeshMap["LionCharacter"] = SK_LION.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LOBSTER
	(TEXT("/Game/model/animals/lobstar/Object_7.Object_7"));
	if (SK_LOBSTER.Succeeded()) {
		monsterMeshMap["LobsterCharacter"] = SK_LOBSTER.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH
	(TEXT("/Game/model/animals/mammoth/sm_mammoth.sm_mammoth"));
	if (SK_MAMMOTH.Succeeded()) {
		monsterMeshMap["MammothCharacter"] = SK_MAMMOTH.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_OCTOPUS
	(TEXT("/Game/model/animals/octopus/Octopus1.Octopus1"));
	if (SK_OCTOPUS.Succeeded()) {
		monsterMeshMap["OctopusCharacter"] = SK_OCTOPUS.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PENGUIN
	(TEXT("/Game/model/animals/penguin/penguin_swimming.penguin_swimming"));
	if (SK_PENGUIN.Succeeded()) {
		monsterMeshMap["PenguinCharacter"] = SK_PENGUIN.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_RABBIT
	(TEXT("/Game/model/animals/rabbit/low_poly_rabbit.low_poly_rabbit"));
	if (SK_RABBIT.Succeeded()) {
		monsterMeshMap["RabbitCharacter"] = SK_RABBIT.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SABERTOOTHTIGER
	(TEXT("/Game/model/animals/sabertoothTiger/sm_sabertoothTiger.sm_sabertoothTiger"));
	if (SK_SABERTOOTHTIGER.Succeeded()) {
		monsterMeshMap["SaberToothTigerCharacter"] = SK_SABERTOOTHTIGER.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SEAL
	(TEXT("/Game/model/animals/seal/seal1.seal1"));
	if (SK_SEAL.Succeeded()) {
		monsterMeshMap["SealCharacter"] = SK_SEAL.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SHEPHERD
	(TEXT("/Game/model/animals/shepherd/stylized_low_poly_german_shepherd.stylized_low_poly_german_shepherd"));
	if (SK_SHEPHERD.Succeeded()) {
		monsterMeshMap["ShepherdCharacter"] = SK_SHEPHERD.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SKUNK
	(TEXT("/Game/model/animals/skunk/low_poly_skunk.low_poly_skunk"));
	if (SK_SKUNK.Succeeded()) {
		monsterMeshMap["SkunkCharacter"] = SK_SKUNK.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SLOTH
	(TEXT("/Game/model/animals/sloth/sm_sloth.sm_sloth"));
	if (SK_SLOTH.Succeeded()) {
		monsterMeshMap["SlothCharacter"] = SK_SLOTH.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SQUID
	(TEXT("/Game/model/animals/squid/Squid1.Squid1"));
	if (SK_SQUID.Succeeded()) {
		monsterMeshMap["SquidCharacter"] = SK_SQUID.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STARFISH
	(TEXT("/Game/model/animals/starFish/star_fish.star_fish"));
	if (SK_STARFISH.Succeeded()) {
		monsterMeshMap["StarFishCharacter"] = SK_STARFISH.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STINGRAY
	(TEXT("/Game/model/animals/stingRay/sting_ray.sting_ray"));
	if (SK_STINGRAY.Succeeded()) {
		monsterMeshMap["StingRayCharacter"] = SK_STINGRAY.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WHALE
	(TEXT("/Game/model/animals/whale/whale1.whale1"));
	if (SK_WHALE.Succeeded()) {
		monsterMeshMap["WhaleCharacter"] = SK_WHALE.Object;
	}
	
	monsterMeshMap["NONE"] = nullptr;
}

bool UMonsterSlotComponent::AddMonsterToSlot(ADPCharacter* character, USkeletalMesh* monster)
{
	if (monstersInSlot.size() >= 5)
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("MonsterSlot is full"));
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
	
	// Set Opaqcity
	// NewMeshComponent->SetMaterial(0, opacityMaterial);
	monstersInSlot.push_back(NewMeshComponent);
	return true;
}

bool UMonsterSlotComponent::AddMonsterToSlot(ADPCharacter* character, const FString& monster_type)
{
	FNetLogger::EditerLog(FColor::Cyan, TEXT("AddMonsterToSlot: %s"), *monster_type);
	if (monsterMeshMap.find(monster_type) == monsterMeshMap.end())
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Monster type not found"));
		return false;
	}
	return AddMonsterToSlot(character, monsterMeshMap[monster_type]);
}

void UMonsterSlotComponent::RemoveMonstersFromSlot()
{
	for (auto monster : monstersInSlot)
	{
		monster->DestroyComponent();
	}
	monstersInSlot.clear();
}
