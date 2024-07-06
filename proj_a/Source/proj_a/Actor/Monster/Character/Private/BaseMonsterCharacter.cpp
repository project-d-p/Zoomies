#include "BaseMonsterCharacter.h"

#include "BaseMonsterAIController.h"
#include "DPGameModeBase.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/BulkDataRegistry.h"

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
	
    SetReplicatingMovement(false);

	this->MonsterId = -1;
	this->index = -1;
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
	int32 id = MonsterId;
	FString MonsterID = FString::FromInt(id);
	if (!FDataHub::monsterData.Contains(MonsterID))
	{
		return ;
	}
	MonsterPosition MonsterData = FDataHub::monsterData[MonsterID];
	FVector CurrentPosition = this->GetActorLocation();
	FVector CurrentVelocity = this->GetCharacterMovement()->Velocity;
	
	FVector Position = FVector(MonsterData.position().x(), MonsterData.position().y(), MonsterData.position().z());
	FVector Velocity = FVector(MonsterData.velocity().x(), MonsterData.velocity().y(), 0);
	FRotator Rotation = FRotator(MonsterData.rotation().x(), MonsterData.rotation().y(), 0);
	
	FVector InterpolatedPosition = FMath::VInterpTo(CurrentPosition, Position, 0.1f, 1.0f);
	FVector InterpolatedVelocity = FMath::VInterpTo(CurrentVelocity, Velocity, 0.1f, 1.0f);
	FRotator FinalRotation = FRotator(0, Rotation.Yaw, 0);

	this->SetActorLocation(InterpolatedPosition);
	this->GetCharacterMovement()->Velocity = InterpolatedVelocity;
	this->SetActorRotation(FinalRotation);
}

void ABaseMonsterCharacter::ScaleCapsuleSize(float ScaleFactor)
{
	UCapsuleComponent* LCC = GetCapsuleComponent();
	if (LCC)
	{
		FVector Scalar = FVector(ScaleFactor, ScaleFactor, ScaleFactor);
		LCC->SetRelativeScale3D(Scalar);
	}
}

void ABaseMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		this->SyncPosition();
	}
}

void ABaseMonsterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseMonsterCharacter, MonsterId, COND_InitialOnly);
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

ABaseMonsterCharacter::~ABaseMonsterCharacter()
{
	if (!HasAuthority())
	{
		if (MonsterId == -1)
		{
			return ;
		}
		// FDataHub::monsterData.Remove(FString::FromInt(MonsterId));
		return ;
	}
	if (GetWorld() == nullptr)
	{
		return ;
	}
	ADPGameModeBase* GM = GetWorld()->GetAuthGameMode<ADPGameModeBase>();
	if (!GM)
	{
		return ;
	}
	if (this->MonsterId == -1 && this->index == -1)
	{
		return ;
	}
	if (GM->monster_controllers_[index] != nullptr)
	{
		GM->monster_controllers_[index] = nullptr;
	}
	GM->empty_monster_slots_.push_back(index);
}
