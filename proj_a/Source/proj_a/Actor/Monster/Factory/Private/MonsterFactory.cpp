#include "MonsterFactory.h"

#include "AvoidPlayerMonsterAIController.h"
#include "BaseMonsterAIController.h"
#include "BaseMonsterCharacter.h"
#include "ChasePlayerMonsterAIController.h"
#include "CrabCharacter.h"
#include "DolphinCharacter.h"
#include "EelCharacter.h"
#include "FNetLogger.h"
#include "FoxCharacter.h"
#include "GiraffeCharacter.h"
#include "LionCharacter.h"
#include "LobsterCharacter.h"
#include "MammothCharacter.h"
#include "OctopusCharacter.h"
#include "PenguinCharacter.h"
#include "RabbitCharacter.h"
#include "SaberToothTigerCharacter.h"
#include "SealCharacter.h"
#include "ShepherdCharacter.h"
#include "SkunkCharacter.h"
#include "SlothCharacter.h"
#include "SquidCharacter.h"
#include "StarFishCharacter.h"
#include "StingRayCharacter.h"
#include "TargetPointMonsterAIController.h"
#include "WhaleCharacter.h"
#include "GameFramework/Actor.h"

// TODO: Currently, all random arrays in this function are hardcoded, but this will be changed later.

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
	
	std::vector MonsterClasses = {
		ACrabCharacter::StaticClass(),
		ALobsterCharacter::StaticClass(),
		AMammothCharacter::StaticClass(),
		AOctopusCharacter::StaticClass(),
		AStarFishCharacter::StaticClass(),
		ASlothCharacter::StaticClass(),
		ADolphinCharacter::StaticClass(),
		AEelCharacter::StaticClass(),
		AFoxCharacter::StaticClass(),
		AGiraffeCharacter::StaticClass(),
		ALionCharacter::StaticClass(),
		APenguinCharacter::StaticClass(),
		// ARabbitCharacter::StaticClass(),
		// ASaberToothTigerCharacter::StaticClass(),
		ASealCharacter::StaticClass(),
		// AShepherdCharacter::StaticClass(),
		// ASkunkCharacter::StaticClass(),
		ASquidCharacter::StaticClass(),
		AStarFishCharacter::StaticClass(),
		AStingRayCharacter::StaticClass(),
		AWhaleCharacter::StaticClass()
	};
	UClass* SelectedMonsterClass = MonsterClasses[FMath::RandRange(0, MonsterClasses.size() - 1)];
	
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

	std::vector AIControllerClasses = {
		ATargetPointMonsterAIController::StaticClass(),
		AAvoidPlayerMonsterAIController::StaticClass(),
		AChasePlayerMonsterAIController::StaticClass()
	};

	int32 RandomIndex = FMath::RandRange(0, AIControllerClasses.size() - 1);
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
	std::vector ScaleFactors = { 1.f, 1.5f, 2.f };
	float SelectedScaleFactor = ScaleFactors[FMath::RandRange(0, ScaleFactors.size() - 1)];
	SpawnedMonster->ScaleCapsuleSize(SelectedScaleFactor);
	AIController->index = idx;
	SpawnedMonster->MonsterId = AIController->GetUniqueID();
	AIController->Possess(SpawnedMonster);
	
	return AIController;
}
