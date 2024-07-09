#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TextBlock.h"
#include "Types/ScoreTypes.h"
#include "ScoreUiPrivate.h"
#include "PrivateScoreManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_A_API UPrivateScoreManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPrivateScoreManager();

	int32 GetPrivatePlayerScore() const;
	void IncreasePrivatePlayerScore(const EPlayerJob& playerJob,const TArray<EAnimal>& animals);
	void IncreasePrivatePlayerScoreByServer(const EPlayerJob& playerJob, const TArray<EAnimal>& animals);
	void UpdatePrivatePlayerScoreUI();
	void SetScoreUI_Private(UScoreUiPrivate* ScoreUI_Private);
	
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UTextBlock* ScoreTextPrivate;
protected:
	virtual void BeginPlay() override;
private:
	int32 PrivatePlayerScore = 0;
	int32 PrivatePlayerBaseScore = 0;
	float PrivatePlayerMulScore = 1;
	TArray<TArray<EAnimal>> CapturedAnimals;
	TArray<FScoreData> ScoreDatas;
	UPROPERTY()
	UScoreUiPrivate* ScoreUI_Private = nullptr;
};
