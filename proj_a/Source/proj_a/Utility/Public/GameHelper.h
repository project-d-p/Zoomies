#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameHelper.generated.h"

UCLASS()
class PROJ_A_API UGameHelper : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION()
	static ADPGameModeBase* GetInGameMode(UWorld* World);
	
	UFUNCTION()
	static AGameStateBase* GetInGameState(UObject* ContextObject);
};