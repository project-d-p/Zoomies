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
private:
	UPROPERTY()
	UTextBlock* PrivateScoreTextBlock = nullptr;
};
