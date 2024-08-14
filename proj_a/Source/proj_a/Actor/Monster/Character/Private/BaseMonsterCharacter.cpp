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
	widgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	widgetComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> WIDGET
	(TEXT("/Game/widget/widget_collect.widget_collect_C"));
	if (WIDGET.Class != nullptr) {
		widgetComponent->SetWidgetClass(WIDGET.Class);
	}

	this->MonsterId = -1;
}

void ABaseMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

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
}

void ABaseMonsterCharacter::SyncPosition()
{
	int32 id = MonsterId;
	FString MonsterID = FString::FromInt(id);
	
	MonsterPosition* MonsterData = FDataHub::monsterData.Find(MonsterID);
	if (!MonsterData)
	{
		return ;
	}
	
	FVector CurrentPosition = this->GetActorLocation();
	FVector CurrentVelocity = this->GetCharacterMovement()->Velocity;
	
	FVector Position = FVector(MonsterData->position().x(), MonsterData->position().y(), MonsterData->position().z());
	FVector Velocity = FVector(MonsterData->velocity().x(), MonsterData->velocity().y(), 0);
	FRotator Rotation = FRotator(MonsterData->rotation().x(), MonsterData->rotation().y(), 0);
	
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

void ABaseMonsterCharacter::SetMeshOpacity(float Opacity)
{
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Mesh Component"));
		return;
	}

	int32 MaterialCount = MeshComponent->GetNumMaterials();
	for (int32 i = 0; i < MaterialCount; ++i)
	{
		UMaterialInterface* Material = MeshComponent->GetMaterial(i);
		if (!Material) continue;

		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
		if (!DynamicMaterial)
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			if (!DynamicMaterial)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create Dynamic Material for index %d"), i);
				continue;
			}
			MeshComponent->SetMaterial(i, DynamicMaterial);
		}
		DynamicMaterial->SetScalarParameterValue(FName("Opacity"), Opacity);
		DynamicMaterial->UpdateCachedData();
	}
	MeshComponent->MarkRenderStateDirty();
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
	SetMeshOpacity(0.5f);
}

ABaseMonsterCharacter::~ABaseMonsterCharacter()
{
	// if (!HasAuthority())
	// {
	// 	if (FDataHub::monsterData.Contains(FString::FromInt(MonsterId)))
	// 		FDataHub::monsterData.Remove(FString::FromInt(MonsterId));
	// }
}
