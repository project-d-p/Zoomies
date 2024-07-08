#include "BaseMonsterCharacter.h"

#include "BaseMonsterAIController.h"
#include "DPCharacter.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
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

	if (widgetComponent)
		widgetComponent->SetVisibility(false);
}

void ABaseMonsterCharacter::SyncPosition()
{
	int32 id = MonsterId;
	FString MonsterID = FString::FromInt(id);
	FNetLogger::LogInfo(TEXT("SyncPosition: %s"), *MonsterID);
	
	MonsterPosition* MonsterData = FDataHub::monsterData.Find(MonsterID);
	if (!MonsterData)
	{
		FNetLogger::LogInfo(TEXT("Monster data does not contain: %s"), *MonsterID);
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
		ABaseMonsterAIController *BMC = Cast<ABaseMonsterAIController>(GetOwner());
		check(BMC)
		BMC->StopMovement();
		OnRep_FaintCharacterMotion();
	}
}

void ABaseMonsterCharacter::OnRep_FaintCharacterMotion() const
{
	FRotator NewRotation = FRotator(0.0f, 0.0f, 90.0f);
	GetCapsuleComponent()->SetRelativeRotation(NewRotation);
	GetMesh()->SetRelativeRotation(NewRotation);
}

ABaseMonsterCharacter::~ABaseMonsterCharacter()
{
	// if (!HasAuthority())
	// {
	// 	if (FDataHub::monsterData.Contains(FString::FromInt(MonsterId)))
	// 		FDataHub::monsterData.Remove(FString::FromInt(MonsterId));
	// }
}
