#include "ResultLevelComponent.h"

#include "BasicInputComponent.h"

UResultLevelComponent::UResultLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InputComponent = CreateDefaultSubobject<UBasicInputComponent>(TEXT("InputComponent"));

	InputComponent->SetLevelComponent(this);
}

void UResultLevelComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void UResultLevelComponent::Deactivate()
{
	Super::Deactivate();
}

void UResultLevelComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UResultLevelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UResultLevelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
