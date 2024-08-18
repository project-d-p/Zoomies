#include "PrivateScoreManager.h"
#include "CalculateScoreByJobs.h"
#include "ScoreUiPrivate.h"

UPrivateScoreManager::UPrivateScoreManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPrivateScoreManager::BeginPlay()
{
	Super::BeginPlay();
}

int32 UPrivateScoreManager::GetPrivatePlayerScore() const
{
	return PrivatePlayerScore;
}

void UPrivateScoreManager::IncreasePrivatePlayerScore(const EPlayerJob& playerJob, const TArray<EAnimal>& animals)
{
	FScoreData newScore = UCalculateScoreByJobs::CalculateScoreByJobs(playerJob, animals);
	PrivatePlayerBaseScore += newScore.baseScore; 
	PrivatePlayerMulScore += newScore.addMulScore;
	PrivatePlayerMulScore *= newScore.mulMulScore;
	PrivatePlayerScore = PrivatePlayerBaseScore * PrivatePlayerMulScore;
	
	CapturedAnimals.Add(animals);
	ScoreDatas.Add(newScore);
	UpdatePrivatePlayerScoreUI();
}

void UPrivateScoreManager::IncreasePrivatePlayerScoreByServer(const EPlayerJob& playerJob, const TArray<EAnimal>& animals)
{
	FScoreData newScore = UCalculateScoreByJobs::CalculateScoreByJobs(playerJob, animals);
	PrivatePlayerBaseScore += newScore.baseScore; 
	PrivatePlayerMulScore += newScore.addMulScore;
	PrivatePlayerMulScore *= newScore.mulMulScore;
	PrivatePlayerScore = PrivatePlayerBaseScore * PrivatePlayerMulScore;
	
	CapturedAnimals.Add(animals);
	ScoreDatas.Add(newScore);
}

void UPrivateScoreManager::UpdatePrivatePlayerScoreUI()
{
    if (ScoreTextPrivate == nullptr || ScoreTextPrivate_Front == nullptr || ScoreTextPrivate_Back == nullptr || ScoreTextPrivate_Job == nullptr || ScoreTextPrivate_Total == nullptr)
    {
        ScoreTextPrivate = ScoreUI_Private->GetPrivateScoreTextBlock();
        ScoreTextPrivate_Front = ScoreUI_Private->GetPrivateScoreTextBlock_Front();
        ScoreTextPrivate_Back = ScoreUI_Private->GetPrivateScoreTextBlock_Back();
        ScoreTextPrivate_Job = ScoreUI_Private->GetPrivateScoreTextBlock_Job();
        ScoreTextPrivate_Total = ScoreUI_Private->GetPrivateScoreTextBlock_Total();
    }
    if (ScoreTextPrivate != nullptr)
    {
        ScoreTextPrivate->SetText(FText::FromString(FString::FromInt(PrivatePlayerScore)));
        ScoreTextPrivate_Front->SetText(FText::FromString(FString::FromInt(PrivatePlayerBaseScore)));
        ScoreTextPrivate_Back->SetText(FText::FromString(FString::FromInt(PrivatePlayerMulScore)));
        ScoreTextPrivate_Total->SetText(FText::FromString(FString::FromInt(PrivatePlayerScore)));
        ScoreTextPrivate_Job->SetText(FText::FromString("Job_0"));
        ApplyShakeEffect(ScoreTextPrivate_Front, ShakeTimerHandle_Private_Front);
        ApplyShakeEffect(ScoreTextPrivate_Back, ShakeTimerHandle_Private_Back);
        ApplyShakeEffect(ScoreTextPrivate_Total, ShakeTimerHandle_Private_Total);
    }
}

FString UPrivateScoreManager::GetScoreBase() const
{
    return FString::FromInt(PrivatePlayerBaseScore);
}

void UPrivateScoreManager::SetScoreUI_Private(UScoreUiPrivate* pScoreUI_Private)
{
    if (pScoreUI_Private == nullptr)
    {
        return;
    }
    ScoreUI_Private = pScoreUI_Private;
}

void UPrivateScoreManager::StartShaking(UTextBlock* TargetText, float ShakeInterval, FTimerHandle& InOutHandle)
{
    if (TargetText && TargetText->GetWorld())
    {
        FVector2D OriginTransform = TargetText->GetRenderTransform().Translation;
        float OriginScale = TargetText->GetRenderTransform().Scale.X;
        float OriginRotation = TargetText->GetRenderTransform().Angle;

        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUFunction(this, FName("ShakeText"), TargetText, OriginTransform, OriginScale, OriginRotation);
        TargetText->GetWorld()->GetTimerManager().SetTimer(InOutHandle, TimerDelegate, ShakeInterval, true);
    }
}

void UPrivateScoreManager::StopShaking(UTextBlock* TargetText, const FVector2D& OriginTransform, float OriginScale, float OriginRotation, FTimerHandle& InOutHandle)
{
    if (TargetText && TargetText->GetWorld())
    {
        TargetText->GetWorld()->GetTimerManager().ClearTimer(InOutHandle);

        FWidgetTransform OriginalTransform;
        OriginalTransform.Translation = OriginTransform;
        OriginalTransform.Scale = FVector2D(OriginScale, OriginScale);
        OriginalTransform.Angle = OriginRotation;
        TargetText->SetRenderTransform(OriginalTransform);
    }
}

void UPrivateScoreManager::ShakeText(UTextBlock* TargetText, const FVector2D& OriginTransform, float OriginScale, float OriginRotation)
{
    if (TargetText)
    {
        float MinScale = 0.8f; 
        float MaxScale = 1.4f;
        float RandomScale = FMath::RandRange(MinScale, MaxScale);
        FVector2D NewScale(RandomScale, RandomScale);

        float RotationAmount = 30.0f;
        float RandomRotation = FMath::RandRange(-RotationAmount, RotationAmount);
        float NewRotation = OriginRotation + RandomRotation;

        FWidgetTransform NewTransform;
        NewTransform.Translation = OriginTransform;
        NewTransform.Scale = NewScale;
        NewTransform.Angle = NewRotation;
        TargetText->SetRenderTransform(NewTransform);
    }
}

void UPrivateScoreManager::ApplyShakeEffect(UTextBlock* TargetText, FTimerHandle& InOutHandle)
{
    if (TargetText && TargetText->GetWorld())
    {
        FVector2D OriginTransform = TargetText->GetRenderTransform().Translation;
        float OriginScale = TargetText->GetRenderTransform().Scale.X;
        float OriginRotation = TargetText->GetRenderTransform().Angle;

        StartShaking(TargetText, 0.07f, InOutHandle);  // Shake interval을 더 빠르게 설정

        FTimerHandle StopShakeTimerHandle;
        FTimerDelegate StopShakeTimerDelegate;

        StopShakeTimerDelegate.BindUFunction(this, FName("StopShaking"), TargetText, OriginTransform, OriginScale, OriginRotation, InOutHandle);
        GetWorld()->GetTimerManager().SetTimer(StopShakeTimerHandle, StopShakeTimerDelegate, 1.0f, false);  // 1초 후 정지
    }
}