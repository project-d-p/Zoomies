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

	/* Seamless Travel : Reuse PlayerController for Client */
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	/* Seamless Travel : Reuse PlayerController for Server */
	virtual void PostSeamlessTravel() override;
	
protected:
	virtual void Logout(AController* Exiting) override;
};
