#include "ResultWidgetActor.h"

#include "DPCalculateWidget.h"
#include "FNetLogger.h"

struct FFinalScoreData;

AResultWidgetActor::AResultWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	RootComponent = WidgetComponent;
}

void AResultWidgetActor::BeginPlay()
{
	Super::BeginPlay();
}

UUserWidget* AResultWidgetActor::GetWidget() const
{
	if (WidgetComponent)
	{
		return WidgetComponent->GetWidget();
	}
	return nullptr;
}

void AResultWidgetActor::StartWidget(const TArray<FFinalScoreData>& InFinalScoreDataArray)
{
	if (WidgetComponent)
	{
		UDPCalculateWidget* CalculateWidget = Cast<UDPCalculateWidget>(GetWidget());
		if (CalculateWidget)
		{
			CalculateWidget->OnScoresUpdated(InFinalScoreDataArray);
		}
	}
}
