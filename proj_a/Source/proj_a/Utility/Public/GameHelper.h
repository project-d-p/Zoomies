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
	static UWorld* GetWorldFromContext(UObject* ContextObject);
	
	UFUNCTION()
	static AGameModeBase* GetGameMode(UObject* ContextObject);
	
	UFUNCTION()
	static AGameStateBase* GetGameState(UObject* ContextObject);
};