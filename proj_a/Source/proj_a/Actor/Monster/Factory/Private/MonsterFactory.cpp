#include "MonsterFactory.h"

#include "AvoidPlayerMonsterAIController.h"
#include "BaseMonsterAIController.h"
#include "BaseMonsterCharacter.h"
#include "ChasePlayerMonsterAIController.h"
#include "CrabCharacter.h"
#include "FNetLogger.h"
#include "LobstarCharacter.h"
#include "MammothCharacter.h"
#include "OctopusCharacter.h"
#include "SlothCharacter.h"
#include "StarFishCharacter.h"
#include "TargetPointMonsterAIController.h"
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
	
	TArray<UClass*> MonsterClasses = {
		ACrabCharacter::StaticClass(),
		ALobstarCharacter::StaticClass(),
		AMammothCharacter::StaticClass(),
		AOctopusCharacter::StaticClass(),
		AStarFishCharacter::StaticClass(),
		ASlothCharacter::StaticClass(),
	};
	UClass* SelectedMonsterClass = MonsterClasses[FMath::RandRange(0, MonsterClasses.Num() - 1)];
	
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

	TArray<UClass*> AIControllerClasses = {
		ATargetPointMonsterAIController::StaticClass(),
		// AAvoidPlayerMonsterAIController::StaticClass(),
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
		FNetLogger::LogError(TEXT("Failed to spawn monster character"));
		AIController->Destroy();
		return nullptr;
	}
	if (SpawnedMonster)
	{
		TArray<float> ScaleFactors = { 0.5f, 1.0f, 2.0f };
		float SelectedScaleFactor = ScaleFactors[FMath::RandRange(0, ScaleFactors.Num() - 1)];
		SpawnedMonster->ScaleCapsuleSize(SelectedScaleFactor);
		SpawnedMonster->index = idx;
	}
	SpawnedMonster->MonsterId = AIController->GetUniqueID();
	AIController->Possess(SpawnedMonster);
	
	return AIController;
}