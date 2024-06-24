#include "MonsterFactory.h"

#include "BaseMonsterAIController.h"
#include "BaseMonsterCharacter.h"
#include "CrabCharacter.h"
#include "FNetLogger.h"
#include "LobstarCharacter.h"
#include "MammothCharacter.h"
#include "OctopusCharacter.h"
#include "SlothCharacter.h"
#include "StarFishCharacter.h"
#include "GameFramework/Actor.h"

ABaseMonsterAIController* UMonsterFactory::RandomMonsterSpawn(const FVector& Location)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		FNetLogger::LogError(TEXT("Invalid world context"));
		return nullptr;
	}

	TArray<UClass*> MonsterClasses = {
		ACrabCharacter::StaticClass(),
		ALobstarCharacter::StaticClass(),
		AMammothCharacter::StaticClass(),
		AOctopusCharacter::StaticClass(),
		AStarFishCharacter::StaticClass(),
		ASlothCharacter::StaticClass(),
	};
	UClass* SelectedMonsterClass = MonsterClasses[FMath::RandRange(0, MonsterClasses.Num() - 1)];
	
	return SpawnMonster(SelectedMonsterClass, Location);
}

ABaseMonsterAIController* UMonsterFactory::SpawnMonster(UClass* MonsterClass, const FVector& Location)
{
	UWorld* World = GetWorld();
	if (!World || !MonsterClass || !MonsterClass->IsChildOf(ABaseMonsterCharacter::StaticClass()))
	{
		FNetLogger::LogError(TEXT("Invalid parameters or class not derived from ABaseMonsterCharacter"));
		return nullptr;
	}
	
	ABaseMonsterAIController* AIController = Cast<ABaseMonsterAIController>(
		World->SpawnActor(ABaseMonsterAIController::StaticClass()));
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
	AIController->Possess(SpawnedMonster);
	return AIController;
}
