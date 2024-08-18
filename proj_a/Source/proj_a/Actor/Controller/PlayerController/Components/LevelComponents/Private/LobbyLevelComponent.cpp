#include "LobbyLevelComponent.h"

#include "DPPlayerController.h"
#include "LobbyInputComponent.h"

ULobbyLevelComponent::ULobbyLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InputComponent = CreateDefaultSubobject<ULobbyInputComponent>(TEXT("InputComponent"));

	InputComponent->SetLevelComponent(this);
}

void ULobbyLevelComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void ULobbyLevelComponent::Deactivate()
{
	Super::Deactivate();
}

void ULobbyLevelComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULobbyLevelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULobbyLevelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
