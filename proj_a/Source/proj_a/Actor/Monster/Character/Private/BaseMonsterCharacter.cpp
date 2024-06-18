#include "BaseMonsterCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseMonsterCharacter::ABaseMonsterCharacter()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCapsuleRadius(0.f, false);
	GetCapsuleComponent()->SetCapsuleHalfHeight(10.f, false);
	
	GetMesh()->SetCollisionProfileName(TEXT("MonsterMesh"));
	GetMesh()->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	
	GetMesh()->SetupAttachment(RootComponent);
    GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, 0.f, 0.f));

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
	
	/* XXX: comment for testing purposes. Restore after creating a UDP structure later. */
    // SetReplicatingMovement(false);
}

void ABaseMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
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
