#include "ReturnTriggerVolume.h"

#include "DPCharacter.h"
#include "FNetLogger.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
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
	
	TriggerSphere->SetRelativeLocation(FVector(-1.185065, 1.450112, -4.687501));
	TriggerSphere->SetSphereRadius(750.0f);
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> EFFECTS_RETURN
	(TEXT("/Game/effect/ns_teleport.ns_teleport"));
	if (EFFECTS_RETURN.Succeeded()) {
		EffectsReturn = EFFECTS_RETURN.Object;
	}
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

void AReturnTriggerVolume::SpawnReturnEffect(TArray<EAnimal> Array)
{
	if (EffectsReturn)
	{
		// Sphere의 중심에서 위쪽으로 반지름만큼 이동한 위치를 계산
		// FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, TriggerSphere->GetScaledSphereRadius());{}
		FVector SpawnLocation = GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			EffectsReturn,
			SpawnLocation,
			GetActorRotation()
		);
	}
}
