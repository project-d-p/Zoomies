#pragma once

#include "CoreMinimal.h"
#include "EnumTypes.h"
#include "PlayerName.generated.h"

USTRUCT(BlueprintType)
struct FPlayerName
{
	GENERATED_BODY()

	static FString GetJobName(const EPlayerJob& Job)
	{
		switch (Job)
		{
		case EPlayerJob::JOB_ENVIRONMENTALIST: return TEXT("ENVIRONMENTALIST");
		case EPlayerJob::JOB_ARCHAEOLOGIST: return TEXT("ARCHAEOLOGIST");
		case EPlayerJob::JOB_POACHER: return TEXT("POACHER");
		case EPlayerJob::JOB_RINGMASTER: return TEXT("RINGMASTER");
		case EPlayerJob::JOB_TERRORIST: return TEXT("TERRORIEST");
		default: return TEXT("Unknown");
		}
	}
};