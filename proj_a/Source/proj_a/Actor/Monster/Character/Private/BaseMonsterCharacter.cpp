#include "BaseMonsterCharacter.h"

#include "DPCharacter.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "proj_a.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

/** Static storage for loaded meshes and animations */
static std::map<EAnimal, USkeletalMesh*> SkeletalMeshMap;
static std::map<EAnimal, USkeletalMesh*> SkeletalMeshTransparencyMap;
static std::map<EAnimal, UClass*> AnimClassMap;

ABaseMonsterCharacter::ABaseMonsterCharacter()
{
    bReplicates = true;
	SetReplicatingMovement(false);
	
    PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	
    GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = false;
    bUseControllerRotationYaw = false;

	// GetCharacterMovement()->PrimaryComponentTick.TickInterval = 0.033f;
	
	// arrowSparkle = CreateDefaultSubobject<UArrowComponent>(TEXT("arrowComponent"));
	// arrowSparkle->SetupAttachment(GetMesh());
	// arrowSparkle->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	// arrowSparkle->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	// arrowSparkle->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

	// static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SPARKLE
	// (TEXT("/Game/effect/ns_sparkle.ns_sparkle"));
	// if (SPARKLE.Succeeded()) {
	// 	sparkleEffect = SPARKLE.Object;
	// }

	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->CustomDepthStencilValue = 2;

	//sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComponent"));
	//sphereComponent->SetupAttachment(GetMesh());
	//sphereComponent->InitSphereRadius(400.f);
	//sphereComponent->SetCollisionProfileName(TEXT("trigger"));

	//sphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseMonsterCharacter::OnSphereBeginOverlap);
	//sphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseMonsterCharacter::OnSphereEndOverlap);

	widgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	widgetComponent->SetupAttachment(GetMesh());
	widgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	widgetComponent->SetDrawSize(FVector2D(500.f, 500.f));
	widgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	widgetComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> WIDGET
	(TEXT("/Game/widget/widget_collect.widget_collect_C"));
	if (WIDGET.Class != nullptr) {
		widgetComponent->SetWidgetClass(WIDGET.Class);
	}

	this->MonsterId = -1;

	GetCharacterMovement()->SetWalkableFloorAngle(60.f);
	GetCharacterMovement()->MaxStepHeight = 200.f;
}

void ABaseMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGI_Zoomies* GI = Cast<UGI_Zoomies>(GetGameInstance());
	if (!HasAuthority())
	{
		if (GI)
		{
			OnHostMigrationDelegate = GI->network_failure_manager_->OnHostMigration().AddUObject(this, &ABaseMonsterCharacter::OnHostMigration);
		}
	}

	UNiagaraComponent* sparkle = nullptr;
	// special animals
	if (sparkleEffect && arrowSparkle) {
		sparkle = UNiagaraFunctionLibrary::SpawnSystemAttached(
			sparkleEffect,
			arrowSparkle,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}

	if (sparkle)
		sparkle->SetVectorParameter(FName("particleBoxSize"),GetMesh()->GetRelativeScale3D() * 100.f);
	
	if (widgetComponent)
		widgetComponent->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_MonsterChannel);
	GetMesh()->SetCollisionObjectType(ECC_MonsterChannel);

	//// monster collision object type
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("Monster"));
	//GetMesh()->SetCollisionProfileName(TEXT("Monster"));
}

void ABaseMonsterCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGI_Zoomies* GI = Cast<UGI_Zoomies>(GetGameInstance());
	if (GI)
	{
		GI->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
}


void ABaseMonsterCharacter::InitMonsterMeshData(EAnimal AT)
{
	/** Loading models */
	if (!SkeletalMeshMap.contains(AT))
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> SK(
			PathManager::GetMonsterPath(AT));
		if (SK.Succeeded())
		{
			SkeletalMeshMap[AT] = SK.Object;
		}
	}
	else
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshMap[AT]);
	}

	/** Load TransparencyMesh */
	if (!SkeletalMeshTransparencyMap.contains(AT))
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshFinder(
			PathManager::GetMonsterTransparencyMeshPath(AT));
		if (SkeletalMeshFinder.Succeeded())
		{
			SkeletalMeshTransparencyMap[AT]= SkeletalMeshFinder.Object;
		}
	}
	else
	{
		OSK = SkeletalMeshTransparencyMap[AT];	
	}

	/** Load Animations */
	if (!AnimClassMap.contains(AT))
	{
		ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
			PathManager::GetMonsterAnimationPath(AT));
		if (ANIM_CHARACTER.Succeeded())
		{
			AnimClassMap[AT] = ANIM_CHARACTER.Class;
		}
	}
	else
	{
		GetMesh()->SetAnimInstanceClass(AnimClassMap[AT]);
	}
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void ABaseMonsterCharacter::SyncPosition()
{
	int32 id = MonsterId;
	FString MonsterID = FString::FromInt(id);

	TOptional<MonsterPosition> MonsterData = FDataHub::GetMonsterData(MonsterID);
	if (!MonsterData.IsSet())
	{
		return;
	}

	MonsterPosition MonsterDataValue = MonsterData.GetValue();
	
	FVector CurrentPosition = this->GetActorLocation();
	FVector CurrentVelocity = this->GetCharacterMovement()->Velocity;
	
	FVector Position = FVector(MonsterDataValue.position().x(), MonsterDataValue.position().y(), MonsterDataValue.position().z());
	FVector Velocity = FVector(MonsterDataValue.velocity().x(), MonsterDataValue.velocity().y(), 0);
	FRotator Rotation = FRotator(MonsterDataValue.rotation().x(), MonsterDataValue.rotation().y(), 0);

	float DeltaTime = GetWorld()->GetDeltaSeconds(); // 현재 프레임의 델타 시간
	constexpr float InterpolationSpeed = 1.0f; // 보간 속도
	FVector InterpolatedPosition = FMath::VInterpTo(CurrentPosition, Position, DeltaTime, InterpolationSpeed);
	FVector InterpolatedVelocity = FMath::VInterpTo(CurrentVelocity, Velocity, DeltaTime, InterpolationSpeed);

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

void ABaseMonsterCharacter::SetCatchable(bool bCond)
{
	if (bCond)
	{
		GetMesh()->CustomDepthStencilValue = 3;
		GetMesh()->MarkRenderStateDirty();	// render 
		widgetComponent->SetVisibility(bCond);
	}
	else
	{
		GetMesh()->CustomDepthStencilValue = 2;
		GetMesh()->MarkRenderStateDirty();
		widgetComponent->SetVisibility(bCond);
	}
}

void ABaseMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentPosition = this->GetActorLocation();
	if (CurrentPosition.Z < -1000.f)
	{
		FNetLogger::LogError(TEXT("Monsters Bad Position: %f %f %f"), CurrentPosition.X, CurrentPosition.Y, CurrentPosition.Z);
		// this->Destroy();
	}
	
	if (!HasAuthority())
	{
		this->SyncPosition();
	}
}

void ABaseMonsterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseMonsterCharacter, MonsterId, COND_InitialOnly);
	DOREPLIFETIME(ABaseMonsterCharacter, CurrentState);
}

void ABaseMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseMonsterCharacter::OnHostMigration(UWorld* World, UDataManager* DataManager)
{
	UGI_Zoomies* GI = Cast<UGI_Zoomies>(GetGameInstance());
	if (GI)
	{
		GI->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
	
}

void ABaseMonsterCharacter::TakeMonsterDamage(float Dmg)
{
	CurrentHp -= Dmg;
	if (CurrentHp <= 0 && CurrentState != EMonsterState::Faint)
	{
		CurrentHp = 0;
		CurrentState = EMonsterState::Faint;
		OnRep_FaintCharacterMotion();
	}
}

void ABaseMonsterCharacter::OnRep_FaintCharacterMotion()
{
	GetCapsuleComponent()->SetCapsuleRadius(FaintCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(FaintCP.HalfHeight);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_MonsterChannel, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_MonsterChannel, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerChannel, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerChannel, ECR_Ignore);
	GetMesh()->SetRelativeTransform(CB_FaintStateMtx);
	GetMesh()->SetSkeletalMesh(OSK);
}

ABaseMonsterCharacter::~ABaseMonsterCharacter()
{
	// if (!HasAuthority())
	// {
	// 	if (FDataHub::monsterData.Contains(FString::FromInt(MonsterId)))
	// 		FDataHub::monsterData.Remove(FString::FromInt(MonsterId));
	// }
}
