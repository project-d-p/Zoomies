#include "ResultLevelComponent.h"

#include "BasicInputComponent.h"
#include "DPPlayerController.h"
#include "MainInputComponent.h"
#include "Camera/CameraActor.h"

UResultLevelComponent::UResultLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InputComponent = CreateDefaultSubobject<UBasicInputComponent>(TEXT("InputComponent"));

	InputComponent->SetLevelComponent(this);
}

void UResultLevelComponent::SetFixedCameraView()
{
	ADPPlayerController* PC2 = Cast<ADPPlayerController>(GetOwner());
	check(PC2)
	ADPCharacter* Char = Cast<ADPCharacter>(PC->GetCharacter());
	check(Char)
	Char->RemoveSpringArm();
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

void UResultLevelComponent::Set_PC(ADPPlayerController* PlayerController)
{
	PC = PlayerController;
	if (!PlayerController->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("UResultLevelComponent::PlayerController is not local"));
	}
	Cast<UBasicInputComponent>(InputComponent)->Set_PC(PlayerController);
}