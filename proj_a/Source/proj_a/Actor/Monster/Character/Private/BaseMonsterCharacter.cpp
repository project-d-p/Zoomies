#include "BaseMonsterCharacter.h"

#include "FNetLogger.h"
#include "Components/ArrowComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseMonsterCharacter::ABaseMonsterCharacter()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
	
	arrowSparkle = CreateDefaultSubobject<UArrowComponent>(TEXT("arrowComponent"));
	arrowSparkle->SetupAttachment(GetMesh());
	arrowSparkle->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	arrowSparkle->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	arrowSparkle->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SPARKLE
	(TEXT("/Game/effect/ns_sparkle.ns_sparkle"));
	if (SPARKLE.Succeeded()) {
		sparkleEffect = SPARKLE.Object;
	}

	/* XXX: comment for testing purposes. Restore after creating a UDP structure later. */
    // SetReplicatingMovement(false);
}

void ABaseMonsterCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FNetLogger::EditerLog(FColor::Green, TEXT("OnBeginOverlap"));
}

void ABaseMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// special animals
	if (sparkleEffect && arrowSparkle) {
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			sparkleEffect,
			arrowSparkle,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
}

void ABaseMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ABaseMonsterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
