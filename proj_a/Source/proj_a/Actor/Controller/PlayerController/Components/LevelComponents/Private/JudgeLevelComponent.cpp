#include "JudgeLevelComponent.h"

#include "BasicInputComponent.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"

UJudgeLevelComp::UJudgeLevelComp()
{
	InputComponent = CreateDefaultSubobject<UBasicInputComponent>(TEXT("InputComponent"));
	InputComponent->SetLevelComponent(this);
	
	if (Cast<ADPPlayerController>(GetOuter()))
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("GetOuter is ADPPlayerController"));
	}
}

void UJudgeLevelComp::CreateJudgeLevelUI()
{
	FNetLogger::EditerLog(FColor::Blue, TEXT("UJudgeLevelComp::CreateJudgeLevelUI"));
	checkf(!JudgeLevelUI, TEXT("JudgeLevelUI already exists"));
	APlayerController* PC = Cast<APlayerController>(GetOuter());
	JudgeLevelUI = CreateWidget<UJudgeLevelUI>(PC, UJudgeLevelUI::StaticClass());
	checkf(JudgeLevelUI, TEXT("Failed to create JudgeLevelUI"));
	JudgeLevelUI->AddToViewport();
}

void UJudgeLevelComp::BeginPlay()
{
	Super::BeginPlay();
}