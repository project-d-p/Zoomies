#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ScoreUI.generated.h"

USTRUCT()
struct FScoreUiInitializer
{
	GENERATED_BODY();

	UPROPERTY()
	UWorld* InWorld;
	UPROPERTY()
	UTextBlock* ScoreText;
	UPROPERTY()
	UTextBlock* Player1ScoreText;
	UPROPERTY()
	UTextBlock* Player2ScoreText;
	UPROPERTY()
	UTextBlock* Player3ScoreText;
};

USTRUCT()
struct FTextBlockStruct
{
	GENERATED_BODY()

	UPROPERTY()
	UTextBlock* TextBlock = nullptr;
	UPROPERTY()
	bool bIsSet = false;
};

UCLASS()
class UScoreUI : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void InitScoreUi(FScoreUiInitializer& Initializer);
	UFUNCTION()
	UTextBlock* FindEmptyTextBlock();
	UFUNCTION()
	UTextBlock* GetPlayerScoreTexts() const;
private:
	UPROPERTY()
	TArray<FTextBlockStruct> PlayerScoreTexts;
	UPROPERTY()
	UTextBlock* ScoreText = nullptr;
};
