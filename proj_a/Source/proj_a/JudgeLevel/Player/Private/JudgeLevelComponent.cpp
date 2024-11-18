#include "JudgeLevelComponent.h"
#include "JudgeInputComponent.h"
#include "JudgePlayerController.h"

UJudgeLevelComponent::UJudgeLevelComponent()
{
	InputComponent = CreateDefaultSubobject<UJudgeInputComponent>(TEXT("InputComponent"));
	InputComponent->Set_LC(this);
}

void UJudgeLevelComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void UJudgeLevelComponent::Deactivate()
{
	Super::Deactivate();
	InputComponent->Deactivate();
}

void UJudgeLevelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UJudgeLevelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::LevelTransition)
	{
		Deactivate();
		DestroyComponent();
	}
}

void UJudgeLevelComponent::Set_PC(AJudgePlayerController* AdpPlayerController)
{
	PC_JudegeLevel = AdpPlayerController;
	if (!AdpPlayerController->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("LC_MatchLobby::PlayerController is not local"));
	}
	InputComponent->Set_PC(AdpPlayerController);
}

void UJudgeLevelComponent::BeginPlay()
{
	Super::BeginPlay();
}

UJudgeInputComponent* UJudgeLevelComponent::GetInputComponent() const
{
	return InputComponent;
}