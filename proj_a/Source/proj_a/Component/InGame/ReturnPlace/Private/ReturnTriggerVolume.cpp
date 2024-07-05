#include "ReturnTriggerVolume.h"

#include "DPCharacter.h"
#include "FNetLogger.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"

AReturnTriggerVolume::AReturnTriggerVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and configure the box component
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RootComponent = TriggerSphere;

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AReturnTriggerVolume::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AReturnTriggerVolume::OnOverlapEnd);
}

void AReturnTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
}

void AReturnTriggerVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADPCharacter* Character = Cast<ADPCharacter>(OtherActor);
	if (!Character)
	{
		return ;
	}
	Character->SetAtReturnPlace(true);
}

void AReturnTriggerVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADPCharacter* Character = Cast<ADPCharacter>(OtherActor);
	if (!Character)
	{
		return ;
	}
	Character->SetAtReturnPlace(false);
}

