#include "MonsterFactory.h"

#include "AvoidPlayerMonsterAIController.h"
#include "BaseMonsterAIController.h"
#include "BaseMonsterCharacter.h"
#include "ChamelionCharacter.h"
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
#include "SaberToothTigerCharacter.h"
#include "SealCharacter.h"
#include "HorseCharacter.h"
#include "SkunkCharacter.h"
#include "SlothCharacter.h"
#include "SquidCharacter.h"
#include "StarFishCharacter.h"
#include "StingRayCharacter.h"
#include "TargetPointMonsterAIController.h"
#include "WhaleCharacter.h"
#include "GameFramework/Actor.h"

// TODO: Currently, all random arrays in this function are hardcoded, but this will be changed later.

struct FAIControllerSizeMapping
{
	TSubclassOf<ABaseMonsterAIController> ControllerClass;
	float ScaleFactor;
};

ABaseMonsterAIController* UMonsterFactory::RandomMonsterSpawn(int32 idx)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		FNetLogger::LogError(TEXT("Invalid world context"));
		return nullptr;
	}

	TArray<FVector> locations = {
	FVector(FMath::RandRange(-7426.120216f, -7940.417014f), FMath::RandRange(-5033.563533f, -6275.821662f), 300.0f),
	FVector(FMath::RandRange(-4601.424277f, -5831.038693f), FMath::RandRange(6661.196862f, 6480.767829f), 300.0f),
	};

	int32 randomIndex = FMath::RandRange(0, locations.Num() - 1);
	
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
		AHorseCharacter::StaticClass(),
		AChamelionCharacter::StaticClass(),
		ASaberToothTigerCharacter::StaticClass(),
		ASealCharacter::StaticClass(),
		ASkunkCharacter::StaticClass(),
		ASquidCharacter::StaticClass(),
		AStarFishCharacter::StaticClass(),
		AStingRayCharacter::StaticClass(),
		AWhaleCharacter::StaticClass()
	}; 
	UClass* SelectedMonsterClass = MonsterClasses[FMath::RandRange(0, MonsterClasses.size() - 1)];
	//return SpawnMonster(SelectedMonsterClass, Location, idx);
	return SpawnMonster(SelectedMonsterClass, locations[randomIndex], idx);
}

ABaseMonsterAIController* UMonsterFactory::SpawnMonster(UClass* MonsterClass, const FVector& Location, int32 idx)
{
	UWorld* World = GetWorld();
	if (!World || !MonsterClass || !MonsterClass->IsChildOf(ABaseMonsterCharacter::StaticClass()))
	{
		FNetLogger::LogError(TEXT("Invalid parameters or class not derived from ABaseMonsterCharacter"));
		return nullptr;
	}

	TArray<FAIControllerSizeMapping> AIControllerSizeMappings = {
		{ATargetPointMonsterAIController::StaticClass(), 2.0f},
		// {AAvoidPlayerMonsterAIController::StaticClass(), 0.5f},
		{AChasePlayerMonsterAIController::StaticClass(), 3.0f}
	};

	int32 RandomIndex = FMath::RandRange(0, AIControllerSizeMappings.Num() - 1);
	UClass* SelectedClass = AIControllerSizeMappings[RandomIndex].ControllerClass;
	float SelectedScaleFactor = AIControllerSizeMappings[RandomIndex].ScaleFactor;
    
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

	SpawnedMonster->ScaleCapsuleSize(SelectedScaleFactor);

	AIController->index = idx;
	SpawnedMonster->MonsterId = AIController->GetUniqueID();
	AIController->Possess(SpawnedMonster);
    
	return AIController;
}
