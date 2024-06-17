#include "BaseMonsterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

ABaseMonsterCharacter::ABaseMonsterCharacter()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = true;
	
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH(TEXT("/Game/model/animals/mammoth/sm_mammoth.sm_mammoth"));
    if (SK_MAMMOTH.Succeeded()) {
        GetMesh()->SetSkeletalMesh(SK_MAMMOTH.Object);
    }

	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetMesh()->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	GetMesh()->SetupAttachment(RootComponent);
    GetMesh()->SetRelativeScale3D(FVector(3.00f, 3.00f, 3.00f));
    GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 0.f, 0.f));

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    // SetReplicatingMovement(false);
    // GetCharacterMovement()->Mass = 0.1f;
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
