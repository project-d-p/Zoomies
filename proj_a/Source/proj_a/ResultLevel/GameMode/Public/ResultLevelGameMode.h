#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ResultLevelGameMode.generated.h"

UCLASS()
class PROJ_A_API AResultLevelGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AResultLevelGameMode();
	virtual ~AResultLevelGameMode() override;
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	
};
