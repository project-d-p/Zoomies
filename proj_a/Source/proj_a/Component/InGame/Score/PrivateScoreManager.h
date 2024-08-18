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

	TArray<TArray<EAnimal>> GetCapturedAnimals() const;
	TArray<FScoreData> GetScoreDatas() const;
	int32 GetPrivatePlayerScore() const;
	void IncreasePrivatePlayerScore(const EPlayerJob& playerJob,const TArray<EAnimal>& animals);
	void IncreasePrivatePlayerScoreByServer(const EPlayerJob& playerJob, const TArray<EAnimal>& animals);
	void UpdatePrivatePlayerScoreUI();
	void SetScoreUI_Private(UScoreUiPrivate* ScoreUI_Private);
	UFUNCTION(BlueprintCallable, Category = "ScoreBox")
	FString GetScoreBase() const;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UTextBlock* ScoreTextPrivate;
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UTextBlock* ScoreTextPrivate_Front;
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UTextBlock* ScoreTextPrivate_Back;
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UTextBlock* ScoreTextPrivate_Job;
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UTextBlock* ScoreTextPrivate_Total;

	UFUNCTION()
	void StartShaking(UTextBlock* TargetText, float ShakeInterval, FTimerHandle& InOutHandle);
	UFUNCTION()
	void StopShaking(UTextBlock* TargetText, const FVector2D& OriginTransform, float OriginScale, float OriginRotation, FTimerHandle& InOutHandle);
	UFUNCTION()
	void ShakeText(UTextBlock* TargetText, const FVector2D& OriginTransform, float OriginScale, float OriginRotation);
	void ApplyShakeEffect(UTextBlock* TargetText, FTimerHandle& InOutHandle);
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
	FTimerHandle ShakeTimerHandle_Private_Front;
	FTimerHandle ShakeTimerHandle_Private_Back;
	FTimerHandle ShakeTimerHandle_Private_Total;
};
