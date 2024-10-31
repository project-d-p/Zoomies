#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ScoreUiPrivate.generated.h"

USTRUCT()
struct FPrivateScoreUiInitializer
{
	GENERATED_BODY();

	UPROPERTY()
	UWorld* InWorld;
	UPROPERTY()
	UTextBlock* ScoreTextPrivate;
	UPROPERTY()
	UTextBlock* ScoreTextPrivate_Front;
	UPROPERTY()
	UTextBlock* ScoreTextPrivate_Back;
	UPROPERTY()
	UTextBlock* ScoreTextPrivate_Job;
	UPROPERTY()
	UTextBlock* ScoreTextPrivate_Total;
};

UCLASS()
class PROJ_A_API UScoreUiPrivate : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void InitScoreUiPrivate(const FPrivateScoreUiInitializer& Initializer);
	UFUNCTION()
	UTextBlock* GetPrivateScoreTextBlock() const;
	UFUNCTION()
	UTextBlock* GetPrivateScoreTextBlock_Front() const;
	UFUNCTION()
	UTextBlock* GetPrivateScoreTextBlock_Back() const;
	UFUNCTION()
	UTextBlock* GetPrivateScoreTextBlock_Job() const;
	UFUNCTION()
	UTextBlock* GetPrivateScoreTextBlock_Total() const;
private:
	UPROPERTY()
	UTextBlock* PrivateScoreTextBlock = nullptr;
	UPROPERTY()
	UTextBlock* PrivateScoreTextBlock_Front = nullptr;
	UPROPERTY()
	UTextBlock* PrivateScoreTextBlock_Back = nullptr;
	UPROPERTY()
	UTextBlock* PrivateScoreTextBlock_Job = nullptr;
	UPROPERTY()
	UTextBlock* PrivateScoreTextBlock_Total = nullptr;
};
