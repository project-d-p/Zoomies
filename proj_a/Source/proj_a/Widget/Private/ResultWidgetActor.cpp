#include "ResultWidgetActor.h"

#include "DPCalculateWidget.h"
#include "FNetLogger.h"

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

void AResultWidgetActor::StartWidget()
{
	if (WidgetComponent)
	{
		UDPCalculateWidget* CalculateWidget = Cast<UDPCalculateWidget>(GetWidget());
		if (CalculateWidget)
		{
			CalculateWidget->OnScoresUpdated();
		}
	}
}
