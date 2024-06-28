// Fill out your copyright notice in the Description page of Project Settings.

#include "CHAR_MatchingLobby.h"
#include "Kismet/KismetMathLibrary.h"
#include "DPHpActorComponent.h"
#include "DPConstructionActorComponent.h"
#include "DPWeaponActorComponent.h"
#include "DPStateActorComponent.h"
#include "DPWeaponGun.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"


// Sets default values
ACHAR_MatchingLobby::ACHAR_MatchingLobby()
{
	bReplicates = true;
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weaponComponent = CreateDefaultSubobject<UDPWeaponActorComponent>(TEXT("WeaponComponent"));
	gun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GUNASSET
	(TEXT("/Game/model/weapon/simpleGun.simpleGun"));
	if (GUNASSET.Succeeded()) {
		gun->SetStaticMesh(GUNASSET.Object);
		gun->SetupAttachment(GetMesh(), TEXT("gunSocket"));
	}
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CHARACTER
	(TEXT("/Game/model/steve/StickManForMixamo.StickManForMixamo"));
	if (SK_CHARACTER.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_CHARACTER.Object);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 270.f, 0.f));
	GetMesh()->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/steve/steveAnimation.steveAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CHARACTER_MONTAGE
	(TEXT("/Game/animation/steve/characterAnimMontage.characterAnimMontage"));
	if (CHARACTER_MONTAGE.Succeeded()) {
		characterMontage = CHARACTER_MONTAGE.Object;
	}
	
	AActor::SetReplicatingMovement(false);
	GetCharacterMovement()->Mass = 0.1f;

	if (LobbyInfoWidgetComponent && LobbyInfoWidgetComponent->IsValidLowLevel())
	{
		LobbyInfoWidgetComponent->DestroyComponent();
		LobbyInfoWidgetComponent = nullptr;
	}
	
	LobbyInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LobbyInfoWidgetComponent"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/widget/widget_LobbyInfo.widget_LobbyInfo_C"));
	if (WidgetClass.Succeeded())
	{
		LobbyInfoWidgetComponent->SetWidgetClass(WidgetClass.Class);
	}
	LobbyInfoWidgetComponent->SetVisibility(true);
	LobbyInfoWidgetComponent->SetWidgetSpace( EWidgetSpace::World);
	LobbyInfoWidgetComponent->SetupAttachment(GetMesh());
	LobbyInfoWidgetComponent->SetRelativeLocation(FVector(0, 0, 650));
	LobbyInfoWidgetComponent->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));
	LobbyInfoWidgetComponent->SetDrawSize(FVector2D(260,100));
	LobbyInfoWidgetComponent->SetRelativeRotation(FRotator(-180, -90, 180));
}

// Called when the game starts or when spawned
void ACHAR_MatchingLobby::BeginPlay()
{
	Super::BeginPlay();

	TSubclassOf<ADPWeapon> gunClass = ADPWeaponGun::StaticClass();
	if (weaponComponent) {
		weaponComponent->AddWeapons(gunClass);
		weaponComponent->Equip(gunClass);
	}
}

// Called every frame
void ACHAR_MatchingLobby::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACHAR_MatchingLobby::StopAimAnimation()
{
	if (characterMontage) {
		isAim = false;
		StopAnimMontage(characterMontage); UE_LOG(LogTemp, Warning, TEXT("StopAimAnimation"));
	}
}

void ACHAR_MatchingLobby::UpdateLobbyInfo()
{
	
}
