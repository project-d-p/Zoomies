#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ResultLevelGameMode.generated.h"

USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PlayerName")
	FString PlayerName;
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerJob")
	FString PlayerJob;
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerScore")
	TArray<int32> Scores;

	UPROPERTY(BlueprintReadWrite, Category = "bIsDetected")
	bool bIsDetected;
};

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

	UPROPERTY(BlueprintReadWrite, Category = "PlayerScores")
	TArray<FPlayerScore> PlayerScores;
	
protected:
	virtual void Logout(AController* Exiting) override;
};
