#include "AISightComponent.h"

#include "AIController.h"
#include "DPCharacter.h"
#include "FNetLogger.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"

UAISightComponent::UAISightComponent()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &UAISightComponent::OnPerceptionUpdated);

	// XXX: 나중에 밖으로 빼자
	ConfigureSight(100.0f, 50.0f, 180.0f);
	ActivateSensing();
}

void UAISightComponent::ConfigureSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees)
{
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius; // 감지 추가범위?
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(5.0f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
}

void UAISightComponent::ActivateSensing()
{
	PerceptionComponent->Activate(true);
}

void UAISightComponent::BeginPlay()
{
	Super::BeginPlay();
	ActivateSensing();
}

void UAISightComponent::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	ADPCharacter* NearestActor = nullptr;
	float MinDistance = FLT_MAX;
	
	for (AActor* Actor : UpdatedActors)
	{
		if (ADPCharacter* DPCharacter = Cast<ADPCharacter>(Actor))
		{
			float Distance = FVector::Dist(DPCharacter->GetActorLocation(), GetOwner()->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestActor = DPCharacter;
			}
		}
	}
	
	if (NearestActor)
	{
		FNetLogger::EditerLog(FColor::Blue, TEXT("Nearest Actor: %s"), *NearestActor->GetName());
		OnEnemyDetected.Broadcast(NearestActor);

		if (AAIController* AIController = Cast<AAIController>(GetOwner()))
		{
				AIController->MoveToActor(NearestActor, 5.0f);
		}
	}
}
