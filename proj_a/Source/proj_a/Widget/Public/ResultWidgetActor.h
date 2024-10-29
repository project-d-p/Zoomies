#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "ResultWidgetActor.generated.h"

UCLASS()
class AResultWidgetActor : public AActor
{
	GENERATED_BODY()
public:
	AResultWidgetActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Result_Widget")
	UWidgetComponent* WidgetComponent;

	UFUNCTION(BlueprintCallable, Category = "Result_Widget")
	UUserWidget* GetWidget() const;

	void StartWidget(const TArray<FFinalScoreData>& InFinalScoreDataArray);
	
protected:
	virtual void BeginPlay() override;
};
