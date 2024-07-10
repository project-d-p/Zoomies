#include "MonsterFactory.h"

#include "AvoidPlayerMonsterAIController.h"
#include "BaseMonsterAIController.h"
#include "BaseMonsterCharacter.h"
#include "ChasePlayerMonsterAIController.h"
#include "CrabCharacter.h"
#include "FNetLogger.h"
#include "LobsterCharacter.h"
#include "MammothCharacter.h"
#include "OctopusCharacter.h"
#include "SlothCharacter.h"
#include "StarFishCharacter.h"
#include "TargetPointMonsterAIController.h"
#include "GameFramework/Actor.h"

// TODO: Currently, all random arrays in this function are hardcoded, but this will be changed later.

UMonsterFactory::UMonsterFactory()
{
	LoadBlueprintClasses();
}

ABaseMonsterAIController* UMonsterFactory::RandomMonsterSpawn(int32 idx)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		FNetLogger::LogError(TEXT("Invalid world context"));
		return nullptr;
	}

	// XXX: For now, hardcoding the Location.
	float RandomY = FMath::FRandRange(-3000.f, 3000.f);
	FVector Location = FVector(-5000.f, RandomY, 300.f);

	UINT RandomRange = FMath::RandRange(0, MonsterBlueprintClasses.Num() - 1);
	UClass* SelectedMonsterClass = MonsterBlueprintClasses[RandomRange];
	
	return SpawnMonster(SelectedMonsterClass, Location, idx);
}

ABaseMonsterAIController* UMonsterFactory::SpawnMonster(UClass* MonsterClass, const FVector& Location, int32 idx)
{
	UWorld* World = GetWorld();
	if (!World || !MonsterClass || !MonsterClass->IsChildOf(ABaseMonsterCharacter::StaticClass()))
	{
		FNetLogger::LogError(TEXT("Invalid parameters or class not derived from ABaseMonsterCharacter"));
		return nullptr;
	}

	TArray AIControllerClasses = {
		ATargetPointMonsterAIController::StaticClass(),
		AAvoidPlayerMonsterAIController::StaticClass(),
		AChasePlayerMonsterAIController::StaticClass()
	};

	int32 RandomIndex = FMath::RandRange(0, AIControllerClasses.Num() - 1);
	UClass* SelectedClass = AIControllerClasses[RandomIndex];
	
	ABaseMonsterAIController* AIController = Cast<ABaseMonsterAIController>(
		World->SpawnActor(SelectedClass));
	if (AIController == nullptr)
	{
		FNetLogger::LogError(TEXT("Failed to spawn AI controller"));
		return nullptr;
	}
	
	ABaseMonsterCharacter* SpawnedMonster = Cast<ABaseMonsterCharacter>(
		World->SpawnActor(MonsterClass, &Location, &FRotator::ZeroRotator));
	if (SpawnedMonster == nullptr)
	{
		FNetLogger::LogError(TEXT("Failed to spawn monster character id %d"), AIController->GetUniqueID());
		AIController->Destroy();
		return nullptr;
	}
	TArray ScaleFactors = { 1.0f, 1.5f, 2.0f };
	float SelectedScaleFactor = ScaleFactors[FMath::RandRange(0, ScaleFactors.Num() - 1)];
	SpawnedMonster->ScaleCapsuleSize(SelectedScaleFactor);
	AIController->index = idx;
	SpawnedMonster->MonsterId = AIController->GetUniqueID();
	AIController->Possess(SpawnedMonster);
	
	return AIController;
}

UClass* UMonsterFactory::LoadBlueprintClass(const TCHAR* Path)
{
	ConstructorHelpers::FClassFinder<AActor> BlueprintClassFinder(Path);
	return BlueprintClassFinder.Class;
}

void UMonsterFactory::LoadBlueprintClasses()
{
	MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/CrabCharacter_BP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/LobsterCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/MammothCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/OctopusCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/StarFishCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/SlothCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/DolphinCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/EelCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/FoxCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/GiraffeCharacterBP")));

	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/LionCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/PenguinCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/RabbitCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/SaberToothTigerCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/SealCharacterBP")));
	
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/ShepherdCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/SkunkCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/SquidCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/StingRayCharacterBP")));
	// MonsterBlueprintClasses.Add(LoadBlueprintClass(TEXT("/Game/Monster/WhaleCharacterBP")));
}
