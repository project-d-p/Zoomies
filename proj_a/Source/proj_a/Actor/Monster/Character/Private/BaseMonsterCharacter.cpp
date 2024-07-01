#include "BaseMonsterCharacter.h"

#include "BaseMonsterPlayerState.h"
#include "FDataHub.h"
#include "FNetLogger.h"
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
}

void ABaseMonsterCharacter::SyncPosition()
{
	ABaseMonsterPlayerState* mPlayerState = Cast<ABaseMonsterPlayerState>(this->GetPlayerState());
	if (mPlayerState == nullptr)
	{
		FNetLogger::LogError(TEXT("PlayerState is nullptr"));
		return;
	}
	int id = mPlayerState->GetPlayerId();
	FString MonsterID = FString::FromInt(id);
	if (!FDataHub::monsterData.Contains(MonsterID))
	{
		return ;
	}
	MonsterPosition MonsterData = FDataHub::monsterData[MonsterID];
	FVector CurrentPosition = this->GetActorLocation();
	FVector CurrentVelocity = this->GetCharacterMovement()->Velocity;
	
	FVector Position = FVector(MonsterData.position().x(), MonsterData.position().y(), CurrentPosition.Z);
	FVector Velocity = FVector(MonsterData.velocity().x(), MonsterData.velocity().y(), 0);
	FRotator Rotation = FRotator(MonsterData.rotation().x(), MonsterData.rotation().y(), 0);
	
	FVector InterpolatedPosition = FMath::VInterpTo(CurrentPosition, Position, 0.1f, 1.0f);
	FVector InterpolatedVelocity = FMath::VInterpTo(CurrentVelocity, Velocity, 0.1f, 1.0f);
	FRotator FinalRotation = FRotator(0, Rotation.Yaw, 0);

	this->SetActorLocation(InterpolatedPosition);
	this->GetCharacterMovement()->Velocity = InterpolatedVelocity;
	this->SetActorRotation(FinalRotation);
}

void ABaseMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		this->SyncPosition();
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
