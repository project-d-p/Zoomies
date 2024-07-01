#include "BaseMonsterCharacter.h"

#include "FNetLogger.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
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

	if (widgetComponent)
		widgetComponent->SetVisibility(false);
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

//void ABaseMonsterCharacter::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (widgetComponent)
//		widgetComponent->SetVisibility(true);
//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("begin overlap"));
//	UE_LOG(LogTemp, Warning, TEXT("begin overlap"));
//}
//
//void ABaseMonsterCharacter::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	if (widgetComponent)
//		widgetComponent->SetVisibility(false);
//}
