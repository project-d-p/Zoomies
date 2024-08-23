#include "MatchLobbyLevelComponent.h"

#include "BasicInputComponent.h"
#include "MainInputComponent.h"
#include "Camera/CameraActor.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"

UMatchLobbyLevelComponent::UMatchLobbyLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InputComponent = CreateDefaultSubobject<UBasicInputComponent>(TEXT("InputComponent"));

	InputComponent->SetLevelComponent(this);
}

void UMatchLobbyLevelComponent::SetFixedCameraView()
{
	APC_MatchingLobby* PC = Cast<APC_MatchingLobby>(GetOwner());
	check(PC)
	ADPCharacter* Char = Cast<ADPCharacter>(PC->GetCharacter());
	check(Char)
	Char->RemoveSpringArm();
}

void UMatchLobbyLevelComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void UMatchLobbyLevelComponent::Deactivate()
{
	Super::Deactivate();
}

void UMatchLobbyLevelComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMatchLobbyLevelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMatchLobbyLevelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
