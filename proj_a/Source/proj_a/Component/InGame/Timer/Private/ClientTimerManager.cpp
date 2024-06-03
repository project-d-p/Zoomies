#include "ClientTimerManager.h"

#include "FNetLogger.h"
#include "Net/UnrealNetwork.h"

UClientTimerManager::UClientTimerManager()
{
	SetIsReplicatedByDefault(true);
	// PrimaryComponentTick.bCanEverTick = true;

	TimeRemaining = 0.0f;
}

void UClientTimerManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UClientTimerManager::Update, 1.0f, true);
}

void UClientTimerManager::SetTimeRemaining(float NewTime)
{
	TimeRemaining = NewTime;
}

void UClientTimerManager::Update()
{
	if (AActor* Owner = GetOwner())
		if (Owner->HasAuthority())
			return;
	TimeRemaining = 10.0f;
	FNetLogger::EditerLog(FColor::Magenta, TEXT("Time remaining updated: %f"), TimeRemaining);
}

void UClientTimerManager::OnRep_TimeRemaining()
{
	FNetLogger::EditerLog(FColor::Magenta, TEXT("Time remaining updated: %f"), TimeRemaining);
}

void UClientTimerManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UClientTimerManager, TimeRemaining);
}